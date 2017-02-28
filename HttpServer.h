#ifndef  ChatRoom_HttpServer_H
#define ChatRoom_HttpServer_H

#include "TcpServer.h"
namespace ChatRoom
{
	class EventLoop;
	class HttpServer
	{
	public:
		HttpServer(EventLoop* loop)
	private:
		EventLoop* loop_;
		TcpServer server_;
	};
}
#endif // ! ChatRoom_HttpServer_H
