#ifndef  ChatRoom_TcpServer_H
#define  ChatRoom_TcpServer_H
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
	class TcpServer
	{
	public:
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		typedef std::function<void(EventLoop*)> ThreadInitialCallback;
		typedef std::function<void(TcpConnectionPtr ptr)> NewConnectionCallback;
		//typedef std::function<void(TcpConnectionPtr ptr)> WriteCompleteCallback();
		typedef std::function<void(TcpConnectionPtr ptr, Buffer* buff)> MessageCallback;
		typedef std::function<void(TcpConnectionPtr ptr)> CloseCallback;
		TcpServer(EventLoop *loop, const struct sockaddr_in * listenAddr, bool reusePort = false);
	
		void setThreadNum(const unsigned int num);

		void setThreadInitialCallback(ThreadInitialCallback & initial);
		void setThreadInitialCallback(ThreadInitialCallback && initial);

		void setNewConnectionCallback(NewConnectionCallback & cb);
		void setNewConnectionCallback(NewConnectionCallback && cb);

		void setMessageCallback(MessageCallback & cb);
		void setMessageCallback(MessageCallback && cb);
		//void setWriteCompleteCallback(WriteCompleteCallback &&cb);
		void setCloseCallback(CloseCallback & cb);
		void setCloseCallback(CloseCallback && cb);

		void start();
	private:
		//For Acceptor 
		void newConnection(int sockfd, const struct sockaddr& clientAddr);
		//For TcpConnection
		void removeConnection(TcpConnectionPtr ptr);
		void removeConnectionInLoop(TcpConnectionPtr ptr);
		EventLoop* loop_;
		struct sockaddr_in listenAddr_;
		bool reusePort_;
		unsigned int threadNum_;
		ThreadInitialCallback threadInitialCallback_;
		std::shared_ptr<Acceptor> acceptor_;
		std::shared_ptr<ThreadPool> threadPool_;
		std::map<std::string, TcpConnectionPtr> conn_;
		NewConnectionCallback newConnectionCallback_;
		MessageCallback messageCallback_;
		//WriteCompleteCallback writeCompleteCallback_;
		CloseCallback closeCallback_;
	};
}
#endif // ! ChatRoom_TcpServer_H
