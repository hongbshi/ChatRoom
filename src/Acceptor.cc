#include "Acceptor.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include <stdio.h>
#include <stdlib.h>
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
	printf("File: Acceptor.cc, Acceptor::Acceptor function, acceptor sockfd is %d.\n", sockfd_);
	const struct sockaddr *addr = sockaddr_cast(listenAddr);
	socklen_t len = sizeof(*addr);
	int res = bindAddress(sockfd_, addr, len);
	if(res < 0){
		printf("File: Acceptor.cc, Acceptor::Acceptor function, bind address failed.\n");
		exit(-1);
	}
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
	int res = listenSocket(sockfd_, SOMAXCONN);
	if(res < 0){
		printf("File: Acceptor.cc, Acceptor::listen function, listen socket failed.\n");
		exit(-1);
	}
	channel_.enableRead();
	loop_->updateChannle(&channel_);
	listening_ = true;
	printf("File: Acceptor.cc, Acceptor::listen function end.\n");
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
	printf("File: Acceptor.cc, Acceptor::handleRead funcion, new connect sockfd is %d.\n", connectFd);
	if (connectFd >= 0)
	{
	    setNonblock(connectFd);
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
