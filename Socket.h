#ifndef  ChatRoom_Socket_H
#define  ChatRoom_Socket_H
//Define usually socket operation.
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
namespace ChatRoom
{
	//socket basic operation
	int createNonblockSocket(int family,int type,int protocol);
	int bindAddress(int sockfd, const struct sockaddr *addr, socklen_t length);
	int connectAddress(int sockfd, const struct sockaddr *addr, socklen_t length);
	int listenSocket(int sockfd, int maxNum);
	int acceptConnect(int sockfd, struct sockaddr *clientAddr, socklen_t length);
	int closeSocket(int sockfd);
	int shutdownSocket(int sockfd, int type);
	ssize_t readFromSocket(int sockfd, void *buff, size_t length);
	ssize_t readvFromSocket(int sockfd, const struct iovec *iov, int iovcnt);
	ssize_t writeToSocket(int sockfd, const void *buff, size_t length);
	ssize_t writevToSocket(int sockfd, const struct iovec *iov, int iovcnt);
	//socket ops
	struct sockaddr* sockaddr_cast(struct sockaddr_in *addr);
	struct sockaddr_in* sockaddr_in_cast(struct sockaddr* addr);
	const struct sockaddr* sockaddr_cast(const struct sockaddr_in *addr);
	const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
	void fromIpPort(const char *ip, const in_port_t port, struct sockaddr *addr);
	void toIpPort(char *ip, int len, in_port_t *port, const struct sockaddr *addr);
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

