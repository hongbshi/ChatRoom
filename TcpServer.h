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
		TcpServer(EventLoop* loop, const struct sockaddr* listenAddr, bool reusePort);
	private:
		void newConnection(int sockfd, struct sockaddr* clientAddr);
		EventLoop* loop_;
		std::shared_ptr<Acceptor> acceptor_;
		ThreadPool threadPool_;
		//std::map<TcpConnection> conn_;
	};
}
#endif // ! ChatRoom_TcpServer_H
