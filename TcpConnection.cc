#include "TcpConnection.h"
#include "Socket.h"
#include<memory>
using namespace ChatRoom;

ChatRoom::TcpConnection::TcpConnection(EventLoop * loop, int sockfd, 
	sockaddr_in localAddress, 
	sockaddr_in peerAddress)
{
	loop_ = loop;
	sockfd_ = sockfd;
	localAddress_ = localAddress;
	peerAddress_ = peerAddress;
	sockState_=kConnecting;
	channel_=std::make_shared<Channel>(sockfd_);
	connectedCallback_=NULL;
	closeCallback_ = NULL;
	writeCallback_ = NULL;
	messageCallback_ = NULL;
	channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, shared_from_this()));
	channel_->setReadCallback(std::bind(&TcpConnection::handleRead, shared_from_this()));
	channel_->setErrorCallback(std::bind(&TcpConnection::handleError, shared_from_this()));
	channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, shared_from_this()));
}
ChatRoom::TcpConnection::~TcpConnection()
{
	::close(sockfd_);
}
// Must in TcpConnection loop
void ChatRoom::TcpConnection::connectEstablished()
{
	sockState_ = kConnected;
	channel_->enableRead();
	loop_->updateChannle(&*channel_);
	if (connectedCallback_)
		connectedCallback_(shared_from_this());
}
// Must in TcpConnection loop
void ChatRoom::TcpConnection::connectDestroyed()
{
	sockState_ = kDisconnected;
	channel_->disableAll();
	loop_->removeChannle(&*channel_);
	TcpConnectionPtr guard(shared_from_this());
	if (connectedCallback_)
		connectedCallback_(guard);
	//tcpserver 提供一个函数 用于 从tcpserver 中删除 tcpconnection智能指针
}

void ChatRoom::TcpConnection::startRead()
{
	loop_->runInLoop(std::bind(&TcpConnection::startReadInLoop, shard_from_this()));
}

void ChatRoom::TcpConnection::stopRead()
{
	loop_->runInLoop(std::bind(&TcpConnection::stopReadInLoop, shard_from_this()));
}

void ChatRoom::TcpConnection::close()
{
	loop_->runInLoop(std::bind(&TcpConnection::closeInLoop, shard_from_this()));
}

void ChatRoom::TcpConnection::shutdownWrite()
{
	loop_->runInLoop(std::bind(&TcpConnection::shutdownWriteInLoop, shard_from_this()));
}

//void ChatRoom::TcpConnection::forceClose()
//{
//}

void ChatRoom::TcpConnection::send(std::string & s)
{
	loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, shared_from_this()));
}

//void ChatRoom::TcpConnection::send(std::string && s)
//{
//}

void ChatRoom::TcpConnection::send(Buffer & buff)
{
	send(buff.getString());
}

//void ChatRoom::TcpConnection::send(Buffer && buff)
//{
//}

void ChatRoom::TcpConnection::handleRead()
{
	//Initial variable
	int Errno;
	//Get read result
	int result=outputBuffer_.ReadSocket(sockfd_, &Errno);
	//Deal result
	if (result > 0 && messageCallback_)
		messageCallback_(shared_from_this());
	else if (result == 0)
	{
		if (sockState_ == kConnected)
			handleClose();
		else
			connectDestroyed();
	}	
	else
	{
		errno = Errno;
		handleError();
	}
}

void ChatRoom::TcpConnection::handleWrite()
{
	int Errno;
	int result=inputBuffer_.WriteSocket(sockfd_, &Errno);
	if (0 < result)
	{
		if (inputBuffer_.Readable() == 0)
		{
			channel_->disableWrite();
			loop_->updateChannle(&*channel_);
		}
		if(writeCallback_)
			writeCallback_(shared_from_this());
	}
	else if (result == 0)
		handleClose();
	else
	{
		errno = Errno;
		handleError();
	}
}

void ChatRoom::TcpConnection::handleError()
{
	//log something
	connectDestroyed();
}

void ChatRoom::TcpConnection::handleClose()
{
	if (sockState_ == kConnected)
	{
		sockState_ = kDisconnecting;
		shutdownWriteInLoop();
	}
	if (closeCallback_)
		closeCallback_(shared_from_this());
}

void ChatRoom::TcpConnection::sendInLoop(std::string & s)
{
	if (sockState_ == kDisconnecting || sockState_ == kDisconnected)
		return;
	inputBuffer_.Append(s);  //try send one time
	channel_->enableWrite();
	loop_->updateChannle(&*channel_);
}

void ChatRoom::TcpConnection::shutdownWriteInLoop()
{
	shutdownSocket(sockfd_, SHUT_WRONLY);
	channel_->disableWrite();
	loop_->updateChannle(&*channel_);
}

void ChatRoom::TcpConnection::closeInLoop()
{
	connectDestroyed();
}

void ChatRoom::TcpConnection::startReadInLoop()
{
	if (channel_->isRead())
		return;
	channel_->enableRead();
	loop_->updateChannle(&*channel_);
}

void ChatRoom::TcpConnection::stopReadInLoop()
{
	if (!channel_->isRead())
		return;
	channel_->disableWrite();
	loop_->updateChannle(&*channel_);
}
