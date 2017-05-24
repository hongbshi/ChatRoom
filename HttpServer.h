#ifndef  ChatRoom_HttpServer_H
#define  ChatRoom_HttpServer_H

#include <functional>
#include "TcpServer.h"
namespace ChatRoom
{
	class EventLoop;
	class HttpRequest;
	class HttpResponse;
	class HttpServer
	{
	public:
		//To 
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		typedef std::function<void(EventLoop*)> ThreadInitial;
		typedef std::function<void(TcpConnectionPtr)> NewConnectionCallback;
		typedef std::function<void(const HttpRequest&, HttpResponse&)> MessageCallback;
		typedef std::function<void(TcpConnectionPtr)> CloseCallback;
		HttpServer(EventLoop* loop, 
			const struct sockaddr* listenAddr,
			bool reusePort = false);
		void setThreadNumber(const unsigned int num);
		void setNewConnectionCallback(NewConnectionCallback& cb) { newConnCb_ = cb; }
		void setMessageCallback(MessageCallback& cb) { messageCb_ = cb; }
		void setCloseCallback(CloseCallback& cb) { closeCb_ = cb; }
		void start();
	private:
		void newCb(TcpConnectionPtr ptr);
		void messageCb(TcpConnectionPtr ptr, Buffer* buff);
		void messageCbHelp(TcpConnectionPtr ptr, const HttpRequest& request);
		void defaultMessageCb(const HttpRequest& request, HttpResponse& response);
		void closeCb(TcpConnectionPtr ptr);
		void threadInitial(EventLoop*);
		EventLoop* loop_;
		struct sockaddr listenAddr_;
		bool reusePort_;
		TcpServer server_;
		unsigned int threadNum_;
		NewConnectionCallback newConnCb_;
		MessageCallback messageCb_;
		CloseCallback closeCb_;
		ThreadInitial threadCb_;
	};
}
#endif // ! ChatRoom_HttpServer_H
