#include "Connector.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"
using namespace ChatRoom;

Connector::Connector(EventLoop *loop, const struct sockaddr_in & server):loop_(loop),serverAddr_(server){
	connCb_ = nullptr;
	state_ = kDisConnected;
}

Connector:: ~Connector(){
	int sockfd = resetChannel();
	closeSocket(sockfd);
}

void Connector::start(){
	loop_->runInLoop(&Connector::startInloop,this);
}

void Connector::stop(){
	loop_->runInLoop(&Connector::stopInloop,this);
}

void Connector::startInloop(){
	{
		std::lock_gurad guard(mu_);
		if(state_ != kDisconnected) return;
		state_ = kConnecting;
	}
	int sockfd = createNonblockSocket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	int result = connectAddress(sockfd, *addr, length);
	if(result < 0){
		//Connect Failed
		//Tye again or not should depending on the error.
		closeSocket(sockfd);
		std::lock_gurad guard(mu_);
		state_ = kDisconnected;
		loop_->runInLoop(&Connector::startInloop,this); //Try again, add timer will be better
	}
	else{
		ch_ = make_shared(Channel(sockfd));
		ch_->setWriteCallback(&Connector::handleWrite,this);
		loop_->updateChannle(ch_);
	}
}

void Connector::stopInloop(){
	{
		std::lock_gurad guard(mu_);
		if(state_ != kConnected) return;
		state_ = kDisconnecting;
	}
	int sockfd = resetChannel();
	closeSocket(sockfd);
	setState(States(kDisconnected));
}

int Connector::resetChannel(){
	int sockfd = ch_.getfd();
	ch_.disableAll();
	loop_.removeChannle(ch_);
	ch_ = nullptr;
	return sockfd;
}

void Connector::handleWrite(){
	int sockfd = resetChannel();
	assert(state_ == kDisconnecting);
	setState(States(kConnected));
	if(connCb_) connCb_(sockfd);
}