#ifndef  ChatRoom_TcpServer_H
#define ChatRoom_TcpServer_H
#include<memory>
#include<map>
namespace ChatRoom
{
	class EventLoop;
	class Acceptor;
	class TcpConnection;
	class ThreadPool;
	class TcpServer
	{
	public:
		typedef std::function<void()> Functor;
		typedef void MessageCallback(struct sockaddr* localAddr, struct sockaddr* peerAddr);
		typedef void NewConnectionCallback();
		typedef void WriteCompleteCallback();
		typedef void CloseCallback();
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		TcpServer(EventLoop* loop, const struct sockaddr* listenAddr, bool reusePort=false);
	
		void setThreadNum(const unsigned int num);
		void setThreadInitialCallback(Functor initial);

		void setNewConnectionCallback(NewConnectionCallback && cb);
		void setMessageCallback(MessageCallback &&cb);
		void setWriteCompleteCallback(WriteCompleteCallback &&cb);
		void setCloseCallback(CloseCallback &&cb);
		void start();
	private:
		//For Acceptor 
		void newConnection(int sockfd, struct sockaddr* clientAddr);
		//For TcpConnection
		void removeConnection(TcpConnectionPtr ptr);
		void removeConnectionInLoop(TcpConnectionPtr ptr);
		EventLoop* loop_;
		struct sockaddr listenAddr_;
		bool reusePort_;
		unsigned int threadNum_;
		Functor threadInitialCallback_;
		std::shared_ptr<Acceptor> acceptor_;
		std::shared_ptr<ThreadPool> threadPool_;
		std::map<TcpConnectionPtr,std::string> conn_;
		NewConnectionCallback newConnectionCallback_;
		MessageCallback messageCallback_;
		WriteCompleteCallback writeCompleteCallback_;
		CloseCallback closeCallback_;
	};
}
#endif // ! ChatRoom_TcpServer_H
