#ifndef  ChatRoom_TcpProxy_H
#define  ChatRoom_TcpProxy_H

#include <map>
#include <memory>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>

namespace ChatRoom
{
	class EventLoop;
	class Acceptor;
	class ThreadPool;
	class TcpProxyClient;
	class TcpProxyServer;

	class TcpProxy
	{
	public:
		typedef std::shared_ptr<TcpProxyClient> TcpProxyClientPtr;
		typedef std::shared_ptr<TcpProxyServer> TcpProxyServerPtr;
		typedef std::function<void(EventLoop*)> ThreadInitialCallback;

		TcpProxy(EventLoop *loop, const std::string & addrConf, bool reusePort = false);
	
		void setThreadNum(const unsigned int num){
			threadNum_ = num;
		}

		void start();
	private:
		//For Acceptor 
		void newConnection(int sockfd, const struct sockaddr & clientAddr);

		//For TcpProxyServer
		void closeServerCb(TcpProxyServerPtr ptr);
		
		//For TcpProxyClient
		void closeClientCb(TcpProxyClientPtr ptr);

		//Variable
		bool reusePort_;
		EventLoop *loop_;
		std::string addrConf_;
		unsigned int threadNum_;
		struct sockaddr_in listenAddr_;
		std::shared_ptr<Acceptor> acceptor_;
		std::shared_ptr<ThreadPool> threadPool_;
		ThreadInitialCallback threadInitialCallback_;
		//
		std::map<std::string, TcpProxyClientPtr> client_;
		std::map<std::string, TcpProxyServerPtr> server_;
	};
}
#endif // ! ChatRoom_TcpProxy_H
