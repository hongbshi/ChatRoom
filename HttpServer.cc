#include <cstdio>

#include "HttpServer.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "HttpContext.h"
#include "HttpResponse.h"

using namespace ChatRoom;
using namespace std::placeholders;
ChatRoom::HttpServer::HttpServer(EventLoop * loop, 
	const struct sockaddr_in * listenAddr, 
	bool reusePort):
	loop_(loop),
	listenAddr_(*listenAddr),
	reusePort_(reusePort),
	server_(loop,listenAddr,reusePort),
	threadNum_(0)
{
	newConnCb_ = nullptr;
	messageCb_ = std::bind(&HttpServer::defaultMessageCb,this,_1,_2);
	closeCb_ = nullptr;
	server_.setNewConnectionCallback(std::bind(&HttpServer::newCb, this,_1));
	server_.setMessageCallback(std::bind(&HttpServer::messageCb, this,_1,_2));
	server_.setCloseCallback(std::bind(&HttpServer::closeCb, this,_1));
	server_.setThreadInitialCallback(std::bind(&HttpServer::threadInitial, this,_1));
}

void ChatRoom::HttpServer::setThreadNumber(unsigned int num)
{
	threadNum_ = num;
	server_.setThreadNum(num);
}

void ChatRoom::HttpServer::start()
{
	server_.start();
	printf("HttpServer start. File: HttpServer.cc, HttpServer::start function.\n");
}

void ChatRoom::HttpServer::newCb(TcpConnectionPtr ptr)
{
	//if (ptr->disConnected())
	printf("New connection %s created! File: HttpServer.cc, HttpServer::newCb function\n",ptr->getName().c_str());
	//else
	//	printf("Connection %s destroyed",ptr->getName());
	ptr->setContext(std::make_shared<HttpContext>());
	if (newConnCb_) newConnCb_(ptr);
}

void ChatRoom::HttpServer::messageCb(TcpConnectionPtr ptr, Buffer * buff)
{
	std::shared_ptr<HttpContext> cnt = 
		std::static_pointer_cast<HttpContext>(ptr ->getContext());
	cnt->parseRequest(buff);
	if (cnt->getAll()) {
		messageCbHelp(ptr, cnt->getRequest());
		cnt->resetRequest();
	}
}

void ChatRoom::HttpServer::messageCbHelp(TcpConnectionPtr ptr, const HttpRequest & request)
{
	if (ptr->connected()) {
		HttpResponse response;
		messageCb_(request, response);
		Buffer buff;
		response.appendToBuffer(buff);
		ptr->send(buff);
	}
}

void ChatRoom::HttpServer::defaultMessageCb(const HttpRequest & request, HttpResponse & response)
{
}

void ChatRoom::HttpServer::closeCb(TcpConnectionPtr ptr)
{
	printf("Connection %s destroyed! HttpServer.cc, HttpServer::closeCb function.\n", ptr->getName().c_str());
	if (closeCb_) closeCb_(ptr);
}

void ChatRoom::HttpServer::threadInitial(EventLoop* loop)
{
	printf("New thread created. HttpServer.cc, HttpServer::threadInitial function.\n");
	if (threadCb_) threadCb_(loop);
}
