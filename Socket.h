#ifndef  ChatRoom_Socket_H
#define ChatRoom_Socket_H
//Define usually socket operation.

#include<sys/socket.h>
#include<sys/uio.h>
namespace ChatRoom
{
	//socket basic operation
	int createNonblockSocket(int family,int type,int protocol);
	int bindAddress(int sockfd, const struct sockaddr* addr, socklen_t length);
	int connectAddress(int sockfd, const struct sockaddr* addr, socklen_t length);
	int listenSocket(int sockfd, int maxNum);
	int acceptConnect(int sockfd, sockaddr* clientAddr, socklen_t length);
	int closeSocket(int sockfd);
	int shutdownSocket(int sockfd, int type);
	ssize_t readSocket(int sockfd, void* buff, size_t length);
	ssize_t readvSocket(int sockfd, struct iovec *iov, int iovcnt);
	ssize_t writeSocket(int sockfd, const void* buff, size_t length);
	ssize_t writevSocket(int sockfd, const struct iovec *iov, int iovcnt);
	//socket ops
	struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);
	struct sockaddr_in* sockaddr_in_cast(struct sockaddr* addr);
	void fromIpPort(const char* ip, const struct in_port_t port, struct sockaddr* addr);
	void toIpPort(char*ip, struct in_port_t* port, const struct sockaddr* addr);
	bool isSelfConnect(int sockfd);

	struct sockaddr getLocalAddr(int sockfd);
	struct sockaddr getPeerAddr(int sockfd);
	int getSocketError(int sockfd);

	int setNonblock(int sockfd);
	void setTcpNoDelay(int sockfd);
	void setKeepLive(int sockfd);
	void setReuseAddr(int sockfd);
	void setReusePort(int sockfd);
}
#endif // ! ChatRoom_Socket_H

