#include "Acceptor.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include <stdio.h>
#include <fcntl.h> 
using namespace ChatRoom;

ChatRoom::Acceptor::Acceptor(EventLoop * loop, 
	const struct sockaddr_in  * listenAddr, 
	bool reusePort):
	sockfd_ (createNonblockSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP)),
	idlefd_ (open("/dev/null", O_RDONLY | O_CLOEXEC)),
	loop_ (loop),
	channel_ (sockfd_),
	newConnectCallback_ (nullptr),
	listening_ (false)
{
	printf("Socket fd is %d. File : Acceptor.cc, Acceptor::Acceptor function.\n", sockfd_);
	const struct sockaddr *addr = sockaddr_cast(listenAddr);
	socklen_t len = sizeof(*addr);
	printf("%d\n", len);
	if(bindAddress(sockfd_, addr, len) < 0)
		printf("BindAddress fail. File : Acceptor.cc, Acceptor::Acceptor function.\n");
	setReuseAddr(sockfd_);
	if (reusePort) setReusePort(sockfd_);
	channel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

void ChatRoom::Acceptor::setNewConnectCallback(NewConnectCallback & cb)
{
	newConnectCallback_ = cb;
}

void ChatRoom::Acceptor::setNewConnectCallback(NewConnectCallback && cb)
{
	newConnectCallback_ = std::move(cb);
}

bool ChatRoom::Acceptor::isListening()
{
	return listening_;
}

void ChatRoom::Acceptor::listen()
{
	if(listenSocket(sockfd_, SOMAXCONN) < 0)
		printf("listenSocket Fail. File: Acceptor.cc, Acceptor::listen function.\n");
	channel_.enableRead();
	loop_->updateChannle(&channel_);
	listening_ = true;
	printf("%s\n", "Acceptor is listening! Acceptor.cc, Acceptor::listen function");
}

ChatRoom::Acceptor::~Acceptor()
{
	channel_.disableAll();
	loop_->removeChannle(&channel_);
	closeSocket(sockfd_);
	close(idlefd_);
}

void ChatRoom::Acceptor::handleRead()
{
	struct sockaddr clientAddr;
	int connectFd = acceptConnect(sockfd_, &clientAddr, sizeof clientAddr);
	printf("ConnectFd is %d. File : Acceptor.cc, Acceptor::handleRead funcion.\n", connectFd);
	if (connectFd >= 0)
	{
		if (newConnectCallback_)
			newConnectCallback_(connectFd, clientAddr);
		else
			closeSocket(connectFd);
	}
	else
	{
		if (errno == EMFILE)
		{
			close(idlefd_);
			idlefd_ = acceptConnect(connectFd, &clientAddr, sizeof clientAddr);
			closeSocket(idlefd_);
			fopen("/dev/null", "r");
		}
	}	
}
