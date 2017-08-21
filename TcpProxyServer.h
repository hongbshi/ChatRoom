#ifndef  ChatRoom_TcpProxyServer_H
#define  ChatRoom_TcpProxyServer_H

#include <map>
#include <memory>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>

namespace ChatRoom
{
	class Buffer;
	class EventLoop;
	class TcpConnection;
	class TcpProxyClient;
	class TcpProxyServer: public std::enable_shared_from_this<TcpProxyServer>
	{
	public:
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		typedef std::shared_ptr<TcpProxyServer> TcpProxyServerPtr;
		typedef std::shared_ptr<TcpProxyClient> TcpProxyClientPtr;

		typedef std::function<void(TcpProxyServerPtr)> CloseCallback;

		TcpProxyServer(EventLoop *loop, 
				int sockfd,
				const struct sockaddr & localAddr, 
				const struct sockaddr & peerAddr,
				const std::string & name);

		~TcpProxyServer();
	
		void setCloseCallback(CloseCallback & cb){
			closeCb_ = cb;
		}

		void setCloseCallback(CloseCallback && cb){
			closeCb_ = std::move(cb);
		}

		static int getNumber(){
			return number_++;
		}

		void setWeakContext(std::shared_ptr<void> ptr){
			weakContext_ = ptr;
		}

		std::weak_ptr<void> getWeakContext(){
			return weakContext_;
		}

		std::string getName(){
			return name_;
		}

		void shutdownWrite();

		void send(Buffer * buff);

	private:
		//For TcpConnection
		void handleNewConnection(TcpConnectionPtr ptr);
		void removeConnection(TcpConnectionPtr ptr);
		void removeConnectionInLoop(TcpConnectionPtr ptr);
		void handleWrite(TcpConnectionPtr ptr);
		void handleMessage(TcpConnectionPtr ptr, Buffer *buff);
		void handleReadZero(TcpConnectionPtr ptr);

		//Variable
		EventLoop* loop_;
		std::string name_;
		TcpConnectionPtr conn_;
		std::weak_ptr<void> weakContext_;

		//Call back
		CloseCallback closeCb_;

		//static variable
		static int number_;
	};
}
#endif // ! ChatRoom_TcpProxyServer_H
