#ifndef  ChatRoom_TcpProxyServer_H
#define  ChatRoom_TcpProxyServer_H

#include <memory>
#include <map>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
namespace ChatRoom
{
	class EventLoop;
	class Acceptor;
	class TcpConnection;
	class ThreadPool;
	class Buffer;
	class TcpProxyServer
	{
	public:
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		typedef std::function<void(EventLoop*)> ThreadInitialCallback;
		typedef std::function<void(TcpConnectionPtr ptr)> NewConnectionCallback;
		typedef std::function<void(TcpConnectionPtr ptr, Buffer* buff)> MessageCallback;
		typedef std::function<void(TcpConnectionPtr ptr)> CloseCallback;
		TcpProxyServer(EventLoop *loop, const std::string & addrConf, bool reusePort = false);
	
		void setThreadNum(const unsigned int num);

		void setThreadInitialCallback(ThreadInitialCallback & initial);
		void setThreadInitialCallback(ThreadInitialCallback && initial);

		void setNewConnectionCallback(NewConnectionCallback & cb);
		void setNewConnectionCallback(NewConnectionCallback && cb);

		void setMessageCallback(MessageCallback & cb);
		void setMessageCallback(MessageCallback && cb);

		void setCloseCallback(CloseCallback & cb);
		void setCloseCallback(CloseCallback && cb);

		void start();
	private:
		//For Acceptor 
		void newConnection(int sockfd, const struct sockaddr & clientAddr);
		//For TcpConnection
		void handleNewConnection(TcpConnectionPtr ptr);
		void removeConnection(TcpConnectionPtr ptr);
		void removeConnectionInLoop(TcpConnectionPtr ptr);
		void handleWrite(TcpConnectionPtr ptr);
		void handleMessage(TcpConnectionPtr ptr, Buffer *buff);
		//For TcpProxyClient
		typedef std::shared_ptr<TcpProxyClient> TcpProxyClientPtr;
		void newClient(TcpProxyClientPtr ptr); 
		void closeClient(TcpProxyClientPtr ptr);
		//Variable
		EventLoop* loop_;
		std::string addrConf_;
		struct sockaddr_in listenAddr_;
		//struct sockaddr_in serverAddr_;
		bool reusePort_;
		unsigned int threadNum_;
		std::shared_ptr<Acceptor> acceptor_;
		std::shared_ptr<ThreadPool> threadPool_;
		std::map<std::string, TcpConnectionPtr> conn_;
		//Call back
		ThreadInitialCallback threadInitialCallback_;
		NewConnectionCallback newConnectionCallback_;
		MessageCallback messageCallback_;
		CloseCallback closeCallback_;
	};
}
#endif // ! ChatRoom_TcpProxyServer_H
