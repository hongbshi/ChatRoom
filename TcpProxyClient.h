#ifndef  ChatRoom_TcpProxyClient_H
#define  ChatRoom_TcpProxyClient_H

#include "Buffer.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <utility>
#include <memory>

namespace ChatRoom
{
	class EventLoop;
	class TcpConnection;
	class Connector;
	class Buffer;
	//Not Thread safe
	class TcpProxyClient: public std::enable_shared_from_this<TcpProxyClient>
	{
	public:
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		typedef std::shared_ptr<TcpProxyClient> TcpProxyClientPtr;
		typedef std::function<void(TcpProxyClientPtr ptr)> NewConnectionCallback;
		//typedef std::function<void(TcpProxyClientPtr ptr)> WriteCallback;
		//typedef std::function<void(TcpProxyClientPtr ptr, Buffer* buff)> MessageCallback;
		typedef std::function<void(TcpProxyClientPtr ptr)> CloseCallback;

		TcpProxyClient(EventLoop * loop, const struct sockaddr_in & server);
		~TcpProxyClient();
		
		void setNewConnectionCallback(NewConnectionCallback & cb){ connCb_ = cb;}
		void setNewConnectionCallback(NewConnectionCallback && cb){ connCb_ = std::move(cb);}
		//void setWriteCallback(WriteCallback & cb){ writeCb_ = cb;}
		//void setWriteCallback(WriteCallback && cb){ writeCb_ = std::move(cb);}
		//void setMessageCallback(MessageCallback & cb){ messCb_ = cb;}
		//void setMessageCallback(MessageCallback && cb){ messCb_ = std::move(cb);}
		void setCloseCallback(CloseCallback & cb){ closeCb_ = cb;}
		void setCloseCallback(CloseCallback && cb){ closeCb_ = std::move(cb);}
		
		void connect();
		void disconnect();
		bool isconnect() {return isConnect_;}

		void send(Buffer *buff);

		void setContext(std::shared_ptr<TcpConnection> ptr){
			context_ = ptr;
		}

		std::weak_ptr<TcpConnection> getContext(){
			return context_;
		}

	private:
		//For Connector 
		void connectorCb(int sockfd);
		//For TcpConnection
		void handleNewConnection(TcpConnectionPtr ptr);
		void handleClose(TcpConnectionPtr ptr);
        void handleMessage(TcpConnectionPtr ptr, Buffer* buff);
		void handleWrite(TcpConnectionPtr ptr);
		//Variable
		EventLoop *loop_;
		struct sockaddr_in serverAddr_;
		struct sockaddr_in localAddr_;
		bool isConnect_;
		std::shared_ptr<Connector> connector_;
		TcpConnectionPtr tcpConn_;
		std::weak_ptr<TcpConnection> context_;
		Buffer inBuff_;
		Buffer outBuff_;
		NewConnectionCallback connCb_;
		//WriteCallback writeCb_;
		//MessageCallback messCb_;
		CloseCallback closeCb_;
	};
}
#endif // ! ChatRoom_TcpProxyClient_H
