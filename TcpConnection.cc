#include "TcpConnection.h"
#include "Socket.h"
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
	channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
	channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
	channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
	channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
}

void ChatRoom::TcpConnection::startRead()
{

}

void ChatRoom::TcpConnection::stopRead()
{
}

void ChatRoom::TcpConnection::shutdown()
{
}

void ChatRoom::TcpConnection::forceClose()
{
}

void ChatRoom::TcpConnection::send(std::string & s)
{

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
}

void ChatRoom::TcpConnection::handleWrite()
{
}

void ChatRoom::TcpConnection::handleError()
{
}

void ChatRoom::TcpConnection::handleClose()
{
}
