#ifndef  ChatRoom_TcpServer_H
#define ChatRoom_TcpServer_H
#include<memory>
#include<map>
namespace ChatRoom
{
	class EventLoop;
	class Acceptor;
	class TcpConnection;
	class TcpServer
	{
	public:
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		TcpServer(EventLoop* loop, const struct sockaddr* listenAddr, bool reusePort);
	private:
		void acceptorNewConnection(int sockfd, struct sockaddr* clientAddr);
		void tcpConnection(TcpConnectionPtr ptr);
		void removeConnection(TcpConnectionPtr ptr);
		EventLoop* loop_;
		std::shared_ptr<Acceptor> acceptor_;
		ThreadPool threadPool_;
		//std::map<TcpConnection> conn_;
		//messageCallback,writeCallback,closeCallback,newConnectionCallback
	};
}
#endif // ! ChatRoom_TcpServer_H
