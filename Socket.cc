#include "Socket.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace ChatRoom;

int ChatRoom::createNonblockSocket(int family,
	int type, 
	int protocol)
{
	int fd = socket(family, type, protocol);
	if (fd < 0) return -1;
	if (setNonblock(fd) < 0) return -1;
	return fd;
}

int ChatRoom::bindAddress(int sockfd, 
	const struct sockaddr * addr, 
	socklen_t length)
{
	printf("File: Socket.cc, ChatRoom::bindAddress function, bind sockfd is %d.\n", sockfd);
	const struct sockaddr_in *tmp = sockaddr_in_cast(addr);
	int port = ntohs(tmp->sin_port);
	printf("File: Socket.cc, ChatRoom::bindAddress function, bind port is %d.\n", port);
	char ip[64];
	inet_ntop(AF_INET,(const void*)(&(tmp->sin_addr)),ip,60);
	printf("File: Socket.cc, ChatRoom::bindAddress function, bind ip is %s.\n", ip);
	return bind(sockfd, addr, length);
}

int ChatRoom::connectAddress(int sockfd, 
	const struct sockaddr * addr, 
	socklen_t length)
{
	return connect(sockfd, addr, length);
}

int ChatRoom::listenSocket(int sockfd, int maxNum)
{
	return listen(sockfd, maxNum);
}

int ChatRoom::acceptConnect(int sockfd, 
	struct sockaddr * clientAddr, 
	socklen_t length)
{
	return accept(sockfd, clientAddr, &length);
}

int ChatRoom::closeSocket(int sockfd)
{
	printf("File: Socket.cc, closeSocket function, close %d socket.\n", sockfd);
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

ssize_t ChatRoom::readvFromSocket(int sockfd, const iovec * iov, int iovcnt)
{
	return readv(sockfd, iov, iovcnt);
}

ssize_t ChatRoom::writeToSocket(int sockfd, const void * buff, size_t length)
{
	printf("File: Socket.cc, writeToSocket function start.\n");
	int ans = write(sockfd, buff, length);
	printf("File: Socket.cc, writeToSocket function, write %d bytes to %d.\n", ans, sockfd);
	if(ans < 0) printf("File: Socket.cc, writeToSocket function, errno is %s.\n", strerror(errno));
	return ans;
}

ssize_t ChatRoom::writevToSocket(int sockfd, const iovec * iov, int iovcnt)
{
	return writev(sockfd, iov, iovcnt);
}

struct sockaddr * ChatRoom::sockaddr_cast(struct sockaddr_in * addr)
{
	return (struct sockaddr*)(addr);
}

struct sockaddr_in * ChatRoom::sockaddr_in_cast(struct sockaddr * addr)
{
	return (struct sockaddr_in*)(addr);
}

const struct sockaddr * ChatRoom::sockaddr_cast(const struct sockaddr_in * addr)
{
	return (const struct sockaddr*)(addr);
}

const struct sockaddr_in * ChatRoom::sockaddr_in_cast(const struct sockaddr * addr)
{
	return (const struct sockaddr_in*)(addr);
}

void ChatRoom::fromIpPort(const char * ip, const in_port_t  port, struct sockaddr * addr)
{
	struct sockaddr_in* tmp = sockaddr_in_cast(addr);
	tmp->sin_family = AF_INET;
	tmp->sin_port = port;
	inet_aton(ip, &(tmp->sin_addr));
}

void ChatRoom::toIpPort(char * ip, int len, in_port_t * port, const struct sockaddr * addr)
{
	const struct sockaddr_in *tmp = sockaddr_in_cast(addr);
	if(port != nullptr) *port = tmp->sin_port;
	char *src = inet_ntoa(tmp->sin_addr);
	if(ip != nullptr && src != nullptr && len > strlen(src)) strcpy(ip,src);
}

bool ChatRoom::isSelfConnect(int sockfd)
{
	struct sockaddr localAddr = getLocalAddr(sockfd);
	struct sockaddr_in* local = sockaddr_in_cast(&localAddr);
	struct sockaddr peerAddr = getPeerAddr(sockfd);
	struct sockaddr_in* peer = sockaddr_in_cast(&peerAddr);
	bool flag = false;
	if (local->sin_family == peer->sin_family &&
		local->sin_port == peer->sin_port &&
		local->sin_addr.s_addr == peer->sin_addr.s_addr)
		flag = true;
	return flag;
}

struct sockaddr ChatRoom::getLocalAddr(int sockfd)
{
	struct sockaddr localAddr;
	socklen_t len = sizeof localAddr;
	getsockname(sockfd, &localAddr, &len);
	return localAddr;
}

struct sockaddr ChatRoom::getPeerAddr(int sockfd)
{
	struct sockaddr peerAddr;
	socklen_t len = sizeof peerAddr;
	getpeername(sockfd, &peerAddr, &len);
	return peerAddr;
}

int ChatRoom::getSocketError(int sockfd)
{
	int opt = 1;
	socklen_t len = sizeof opt;
	//result deal
	if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)&opt, &len) < 0) return errno;
	else return opt;
}

int ChatRoom::setNonblock(int sockfd)
{
	int flag;
	flag = fcntl(sockfd, F_GETFL, 0);
	if (flag < 0) return -1;
	flag |= O_NONBLOCK;
	if (fcntl(sockfd, F_SETFL, flag) < 0) return -1;
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
