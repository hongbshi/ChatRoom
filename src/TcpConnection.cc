#include "TcpConnection.h"
#include "Socket.h"

#include <memory>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

using namespace ChatRoom;
using namespace std::placeholders;

int ChatRoom::TcpConnection::number = 1;

ChatRoom::TcpConnection::TcpConnection(EventLoop * loop, 
	int sockfd, 
	const struct sockaddr_in * localAddress, 
	const struct sockaddr_in * peerAddress,
	const std::string & name):
	loop_(loop),
	sockfd_(sockfd),
	localAddress_(*localAddress), 
	peerAddress_(*peerAddress),
	name_(name),
	sockState_(kConnecting), 
	channel_(std::make_shared<Channel>(sockfd_)),
	context_()
{
	connectedCallback_= nullptr;
	closeCallback_ = nullptr;
	writeCallback_ = nullptr;
	messageCallback_ = std::bind(&TcpConnection::defaultMessageCb, _1, _2);
	channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
	channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
	channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
	channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
}

ChatRoom::TcpConnection::~TcpConnection()
{
	closeSocket(sockfd_);
}
// Must in TcpConnection loop
void ChatRoom::TcpConnection::connectEstablished()
{
	printf("File: TcpConnection.cc, TcpConnection::connectEstablished function start.\n");
	assert(sockState_ == kConnecting);
	setStates(kConnected);
	channel_->enableRead();
	channel_->setWeakContext(shared_from_this());
	loop_->updateChannle(&*channel_);
	//setKeepLive(sockfd_);
	if (connectedCallback_) connectedCallback_(shared_from_this());
	printf("File: TcpConnection.cc, TcpConnection::connectEstablished function end.\n");
}

// Must in TcpConnection loop
void ChatRoom::TcpConnection::connectDestroyed()
{
	printf("File: TcpConnection.cc, connectDestroyed function start.\n");
	if (sockState_ == kConnected || sockState_ == kDisconnecting)
	{
		channel_->disableAll();
		loop_->updateChannle(&*channel_);
	}
	setStates(kDisconnected);
	loop_->removeChannle(&*channel_);
	printf("File: TcpConnection.cc, connectDestroyed function end.\n");
}

void ChatRoom::TcpConnection::startRead(){
	loop_->runInLoop(std::bind(&TcpConnection::startReadInLoop, shared_from_this()));
}

void ChatRoom::TcpConnection::stopRead(){
	loop_->runInLoop(std::bind(&TcpConnection::stopReadInLoop, shared_from_this()));
}

void ChatRoom::TcpConnection::startWrite(){
	loop_->runInLoop(std::bind(&TcpConnection::startWriteInLoop, shared_from_this()));
}

void ChatRoom::TcpConnection::stopWrite(){
	loop_->runInLoop(std::bind(&TcpConnection::stopWriteInLoop,shared_from_this()));
}

void ChatRoom::TcpConnection::close(){
	loop_->runInLoop(std::bind(&TcpConnection::closeInLoop, shared_from_this()));
}

void ChatRoom::TcpConnection::shutdownWrite(){
	if(sockState_ == kConnected){
		setStates(kDisconnecting);
		loop_->runInLoop(std::bind(&TcpConnection::shutdownWriteInLoop, shared_from_this()));
	}
}

//void ChatRoom::TcpConnection::forceClose()
//{
//}

void ChatRoom::TcpConnection::send(std::string & s){
	loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, shared_from_this(), s));
}

void ChatRoom::TcpConnection::send(std::string && s){
	loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, shared_from_this(),std::move(s)));
}

void ChatRoom::TcpConnection::send(const Buffer & buff){
	send(buff.getString());
}

void ChatRoom::TcpConnection::send(Buffer & buff){
	std::string str;
	buff.readFromBuffer(str);
	send(std::move(str));
}

void ChatRoom::TcpConnection::handleRead()
{
	//Get read result
	if(sockState_ == kDisconnected) return;
	int savedErrno, result = outputBuffer_.readSocket(sockfd_, &savedErrno);
	printf("File: TcpConnection.cc, TcpConnection::handleRead function, Read %d bytes.\n", result);
	//Deal result
	if (result > 0) messageCallback_(shared_from_this(), &outputBuffer_);
	else if (result == 0){
		if(readZeroCallback_ && sockState_ == kConnected) readZeroCallback_(shared_from_this());
		else handleClose();	
	}
	else {
		errno = savedErrno;
		handleError();
	}
}

void ChatRoom::TcpConnection::handleWrite()
{
	printf("File: TcpConnection.cc, TcpConnection::handleWrite function.\n");
	if(!channel_->isWrite()) return;
	int savedErrno, result = inputBuffer_.writeSocket(sockfd_, &savedErrno);
	printf("File: TcpConnection.cc, TcpConnection::handleWrite function, write %d bytes.\n", result);
	if (result >= 0)
	{
		if (inputBuffer_.readable() == 0)
		{
			channel_->disableWrite();
			loop_->updateChannle(&*channel_);
			if(sockState_ == kDisconnecting){
				shutdownSocket(sockfd_, SHUT_WR);
			}
		}
		if(writeCallback_) writeCallback_(shared_from_this());
	}
	else
	{
		errno = savedErrno;
		handleError();
	}
	printf("File: TcpConnection.cc, TcpConnection::handleWrite function end.\n");
}

void ChatRoom::TcpConnection::handleError()
{
	printf("File: TcpConnection.cc, TcpConnection::handleError function, Error is %s.\n", strerror(errno));
	if((errno == EAGAIN) || (errno == EINPROGRESS) || (errno == EINTR)) return;
	else handleClose();
}

void ChatRoom::TcpConnection::handleClose()
{
	printf("File: TcpConnection.cc, handleclose function.\n");
	assert(sockState_ == kConnected || sockState_ == kDisconnecting);
	if(!outputBuffer_.isEmpty()){
		printf("File: TcpConnetion.cc, handleClose function, %d was left in oBuff.\n", outputBuffer_.readable());
	}
	if(!inputBuffer_.isEmpty()){
		printf("File: TcpConnection.cc, handleClose function, %d was left in iBuff.\n", inputBuffer_.readable());
	}
	channel_->disableAll();
	loop_->updateChannle(&*channel_);
	setStates(kDisconnected);
	TcpConnectionPtr guard(shared_from_this());
	if (closeCallback_) closeCallback_(guard);
}

void ChatRoom::TcpConnection::sendInLoop(std::string & s)
{
	printf("File: TcpConnection.cc, sendInLoop function start.\n");
	if (sockState_ == kDisconnected) return;
	inputBuffer_.writeToBuffer(s);  
	//int savedErrno, result = inputBuffer_.writeSocket(sockfd_, &savedErrno); //try send one time
	//if(result < 0 && errno != EAGAIN){
	//	printf("File: TcpConnection.cc, sendInLoop function, error.\n");
	//	channel_->disableWrite();
	//	loop_->updateChannle(&*channel_);
	//	setStates(kDisconnecting);
	//	return;
	//}
	if(!inputBuffer_.isEmpty()) {
		channel_->enableWrite();
		loop_->updateChannle(&*channel_);
	}
	printf("File: TcpConnection.cc, leave sendInLoop function.\n");
}

void ChatRoom::TcpConnection::shutdownWriteInLoop(){
	if(!channel_->isWrite()) {
		shutdownSocket(sockfd_, SHUT_WR);
	}
}

void ChatRoom::TcpConnection::closeInLoop(){
	//setStates(kDisconnecting);
	handleClose();
}

void ChatRoom::TcpConnection::startReadInLoop(){
	if (channel_->isRead())	return;
	channel_->enableRead();
	loop_->updateChannle(&*channel_);
}

void ChatRoom::TcpConnection::stopReadInLoop(){
	if (!channel_->isRead()) return;
	channel_->disableRead();
	loop_->updateChannle(&*channel_);
}

void ChatRoom::TcpConnection::startWriteInLoop(){
	if(channel_->isWrite()) return;
	channel_->enableWrite();
	loop_->updateChannle(&*channel_);
}

void ChatRoom::TcpConnection::stopWriteInLoop(){
	if(!channel_->isWrite()) return;
	channel_->disableWrite();
	loop_->updateChannle(&*channel_);
}

void ChatRoom::TcpConnection::defaultMessageCb(TcpConnectionPtr ptr, Buffer * buff){
	buff->clear();
}
