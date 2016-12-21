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

void ChatRoom::TcpConnection::connectEstablished()
{
	sockState_ = kConnected;
	channel_->enableRead();
	loop_->updateChannle(&*channel_);
	if (connectedCallback_)
		connectedCallback_(shared_from_this());
}

void ChatRoom::TcpConnection::connectDestroyed()
{

}


//void ChatRoom::TcpConnection::startRead()
//{
//}

//void ChatRoom::TcpConnection::stopRead()
//{
//}

void ChatRoom::TcpConnection::close()
{

}

void ChatRoom::TcpConnection::shutdownWrite()
{

}



void ChatRoom::TcpConnection::forceClose()
{
}

void ChatRoom::TcpConnection::send(std::string & s)
{
	inputBuffer_.Append(s);
	channel_->enableWrite();
	loop_->updateChannle(&*channel_);
}

void ChatRoom::TcpConnection::send(std::string && s)
{
}

void ChatRoom::TcpConnection::send(Buffer & buff)
{
}

void ChatRoom::TcpConnection::send(Buffer && buff)
{
}



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
		handleClose();
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
			loop_->updateChannle(channel_);
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

}

void ChatRoom::TcpConnection::handleClose()
{

	if (closeCallback_)
		closeCallback_(shared_from_this());
}
