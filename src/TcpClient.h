#ifndef  ChatRoom_TcpClient_H
#define  ChatRoom_TcpClient_H
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
	class TcpClient
	{
	public:
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		typedef std::function<void(TcpConnectionPtr ptr)> NewConnectionCallback;
		typedef std::function<void(TcpConnectionPtr ptr)> WriteCallback;
		typedef std::function<void(TcpConnectionPtr ptr, Buffer* buff)> MessageCallback;
		typedef std::function<void(TcpConnectionPtr ptr)> CloseCallback;

		TcpClient(EventLoop * loop, const struct sockaddr_in & server);
		~TcpClient();
		
		void setNewConnectionCallback(NewConnectionCallback & cb){ connCb_ = cb;}
		void setNewConnectionCallback(NewConnectionCallback && cb){ connCb_ = std::move(cb);}
		void setWriteCallback(WriteCallback & cb){ writeCb_ = cb;}
		void setWriteCallback(WriteCallback && cb){ writeCb_ = std::move(cb);}
		void setMessageCallback(MessageCallback & cb){ messCb_ = cb;}
		void setMessageCallback(MessageCallback && cb){ messCb_ = std::move(cb);}
		void setCloseCallback(CloseCallback & cb){ closeCb_ = cb;}
		void setCloseCallback(CloseCallback && cb){ closeCb_ = std::move(cb);}
		
		void connect();
		void disconnect();
		bool isconnect() {return isConnect_;}

	private:
		void connectorCb(int sockfd);
		void handleClose(TcpConnectionPtr ptr);
		void handleWrite(TcpConnectionPtr ptr);
        void handleMessage(TcpConnectionPtr ptr, Buffer* buff);
		EventLoop *loop_;
		struct sockaddr_in serverAddr_;
		struct sockaddr_in localAddr_;
		//int sockfd_;
		bool isConnect_;
		std::shared_ptr<Connector> connector_;
		TcpConnectionPtr tcpConn_;
		NewConnectionCallback connCb_;
		WriteCallback writeCb_;
		MessageCallback messCb_;
		CloseCallback closeCb_;
	};
}
#endif // ! ChatRoom_TcpClient_H
