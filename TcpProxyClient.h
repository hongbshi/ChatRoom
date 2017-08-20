#ifndef  ChatRoom_TcpProxyClient_H
#define  ChatRoom_TcpProxyClient_H

#include "Buffer.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <utility>
#include <memory>
#include <mutex>

#include <string>

using std::string;

namespace ChatRoom
{
	class EventLoop;
	class TcpConnection;
	class Connector;
	class TcpProxyServer;
	//Not Thread safe
	class TcpProxyClient: public std::enable_shared_from_this<TcpProxyClient>
	{
	public:
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		typedef std::shared_ptr<TcpProxyClient> TcpProxyClientPtr;
		typedef std::shared_ptr<TcpProxyServer> TcpProxyServerPtr;
		typedef std::function<void(TcpProxyClientPtr ptr)> NewConnectionCallback;
		typedef std::function<void(TcpProxyClientPtr ptr)> CloseCallback;

		enum State{
			kDisconnecting, kDisconnected, kConnecting, kConnected
		};

		TcpProxyClient(EventLoop * loop, const struct sockaddr_in & server, const string & name);
		~TcpProxyClient();
		
		void setNewConnectionCallback(NewConnectionCallback & cb){ connCb_ = cb;}
		void setNewConnectionCallback(NewConnectionCallback && cb){ connCb_ = std::move(cb);}
		void setCloseCallback(CloseCallback & cb){ closeCb_ = cb;}
		void setCloseCallback(CloseCallback && cb){ closeCb_ = std::move(cb);}
		
		void connect();
		void disconnect();

		bool isconnect() {
			std::lock_guard<std::mutex> guard(mu_);
			return connState_ == kConnected;
		}

		void send(Buffer *buff);

		void setWeakContext(std::shared_ptr<void> ptr){
			weakContext_ = ptr;
		}

		std::weak_ptr<void> getWeakContext(){
			return weakContext_;
		}

		static int getNumber(){
			return number_++;
		}

		std::string getName(){
			return name_;
		}

		void setState(State s){
			std::lock_guard<std::mutex> guard(mu_);
		    connState_ = s;	
		}

		void shutdownWrite();

	private:
		//For Connector 
		void connectorCb(int sockfd);

		//For TcpConnection
		void handleNewConnection(TcpConnectionPtr ptr);
		void handleClose(TcpConnectionPtr ptr);
        void handleMessage(TcpConnectionPtr ptr, Buffer* buff);
		void handleWrite(TcpConnectionPtr ptr);

		//Variable
		string name_;
		Buffer inBuff_;
		Buffer outBuff_;
		EventLoop *loop_;
		struct sockaddr_in localAddr_;
		struct sockaddr_in serverAddr_;

		//State
		std::mutex mu_;
		State connState_;

		//
		TcpConnectionPtr tcpConn_;
		std::weak_ptr<void> weakContext_;
		std::shared_ptr<Connector> connector_;

		//Callback
		NewConnectionCallback connCb_;
		CloseCallback closeCb_;

		//static variable
		static int number_;
	};
}
#endif // ! ChatRoom_TcpProxyClient_H
