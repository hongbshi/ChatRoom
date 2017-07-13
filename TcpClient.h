#ifndef  ChatRoom_TcpClient_H
#define  ChatRoom_TcpClient_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <utility>

namespace ChatRoom
{
	class EventLoop;
	class Connector;
	class TcpClient
	{
	public:
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		typedef std::function<void(TcpConnectionPtr ptr)> NewConnectionCallback;
		typedef std::function<void(TcpConnectionPtr ptr, Buffer* buff)> MessageCallback;
		typedef std::function<void(TcpConnectionPtr ptr)> CloseCallback;

		TcpClient(EventLoop * loop, const struct sockaddr_in & server);
		~TcpClient();
		
		void setNewConnectionCallback(NewConnectionCallback & cb){ connCb_ = cb;}
		void setNewConnectionCallback(NewConnectionCallback && cb){ connCb_ = std::move(cb);}
		void setMessageCallback(MessageCallback & cb){ messCb_ = cb;}
		void setMessageCallback(MessageCallback && cb){ messCb_ = std::move(cb);}
		void setCloseCallback(CloseCallback & cb){ closeCb_ = cb;}
		void setCloseCallback(CloseCallback && cb){ closeCb_ = std::move(cb);}
		
		void connect();
		void disconnect();
		bool isconnect() {return isconnect_;}

	private:
		void connectorCb(int sockfd);
		EventLoop *loop_;
		struct sockaddr_in serverAddr_;
		struct sockaddr_in localAddr_;
		//int sockfd_;
		bool isconnect_;
		std::shared<Connector> connector_;
		TcpConnectionPtr tcpConn_;
		NewConnectionCallback connCb_;
		MessageCallback messCb_;
		CloseCallback closeCb_;
	};
}
#endif // ! ChatRoom_TcpClient_H
