#include "Socket.h"

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
using namespace ChatRoom;

int ChatRoom::createNonblockSocket(int family,
	int type, 
	int protocol)
{
	int fd=socket(family, type, protocol);
	if (fd < 0) return -1;
	if (setNonblock(fd) < 0) return -1;
	return fd;
}

int ChatRoom::bindAddress(int sockfd, 
	const sockaddr * addr, 
	socklen_t length)
{
	return bind(sockfd, addr, length);
}

int ChatRoom::connectAddress(int sockfd, 
	const sockaddr * addr, 
	socklen_t length)
{
	return connect(sockfd, addr, length);
}

int ChatRoom::listenSocket(int sockfd, int maxNum)
{
	return listen(sockfd, maxNum);
}

int ChatRoom::acceptConnect(int sockfd, 
	sockaddr * clientAddr, 
	socklen_t length)
{
	return accept(sockfd, clientAddr, &length);
}

int ChatRoom::closeSocket(int sockfd)
{
	return close(sockfd);
}

int ChatRoom::shutdownSocket(int sockfd, int type)
{
	return shutdown(sockfd, type);
}

ssize_t ChatRoom::readFromSocket(int sockfd, void * buff, size_t length)
{
	return read(sockfd,buff,length);
}

ssize_t ChatRoom::readvFromSocket(int sockfd, iovec * iov, int iovcnt)
{
	return readv(sockfd, iov, iovcnt);
}

ssize_t ChatRoom::writeToSocket(int sockfd, const void * buff, size_t length)
{
	return write(sockfd, buff, length);
}

ssize_t ChatRoom::writevToSocket(int sockfd, const iovec * iov, int iovcnt)
{
	return writev(sockfd, iov, iovcnt);
}

sockaddr * ChatRoom::sockaddr_cast(sockaddr_in * addr)
{
	return (sockaddr*)(addr);
}

sockaddr_in * ChatRoom::sockaddr_in_cast(sockaddr * addr)
{
	return (sockaddr_in*)(addr);
}

void ChatRoom::fromIpPort(const char * ip, const in_port_t  port, sockaddr * addr)
{
	sockaddr_in* tmp = sockaddr_in_cast(addr);
	tmp->sin_family = AF_INET;
	tmp->sin_port = port;
	inet_aton(ip, &(tmp->sin_addr));
}

void ChatRoom::toIpPort(char * ip, in_port_t * port, const sockaddr * addr)
{
	sockaddr_in* tmp = sockaddr_in_cast(addr);
	if(ip!=nullptr)
		*ip = tmp->sin_addr;
	if(port!=nullptr)
		*port = tmp->sin_port;
}

bool ChatRoom::isSelfConnect(int sockfd)
{
	struct sockaddr localAddr = getLocalAddr(sockfd);
	sockaddr_in* local = sockaddr_in_cast(localAddr);
	struct sockaddr peerAddr = getPeerAddr(sockfd);
	sockaddr_in* peer = sockaddr_in_cast(peerAddr);
	bool flag = false;
	if (local->sin_family == peer->sin_family &&
		local->sin_port == peer->sin_port &&
		local->sin_addr == peer->sin_addr)
		flag = true;
	return flag;
}

sockaddr ChatRoom::getLocalAddr(int sockfd)
{
	struct sockaddr localAddr;
	getsockname(sockfd, &localAddr, sizeof localAddr);
	return localAddr;
}

sockaddr ChatRoom::getPeerAddr(int sockfd)
{
	struct sockaddr peerAddr;
	getpeername(sockfd, &peerAddr, sizeof peerAddr);
	return peerAddr;
}

int ChatRoom::getSocketError(int sockfd)
{
	int opt = 1;
	//result deal
	if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)&opt, sizeof opt) < 0)
		return errno;
	else
		return opt;
}

int ChatRoom::setNonblock(int sockfd)
{
	int flag;
	flag = fcntl(sockfd, F_GETFL, 0);
	if (flag < 0) return -1;
	flag |= O_NONBLOCK;
	if (::fcntl(sockfd, F_SETFL, flag) < 0) return -1;
	return 1;
}

void ChatRoom::setTcpNoDelay(int sockfd)
{
	int opt = 1;
	//result deal
	setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (void*)&opt, sizeof opt);
}

void ChatRoom::setKeepLive(int sockfd)
{
	int opt = 1;
	//result deal
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&opt, sizeof opt);
}

void ChatRoom::setReuseAddr(int sockfd)
{
	int opt = 1;
	//result deal
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&opt, sizeof opt);
}

void ChatRoom::setReusePort(int sockfd)
{
	int opt = 1;
	//result deal
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (void*)&opt, sizeof opt);
}
