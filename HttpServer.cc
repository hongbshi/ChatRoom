#include <cstdio>

#include "HttpServer.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "HttpContext.h"
#include "HttpResponse.h"
ChatRoom::HttpServer::HttpServer(EventLoop * loop, 
	const sockaddr * listenAddr, 
	bool reusePort):
	loop_(loop),
	listenAddr_(*listenAddr),
	reusePort_(reusePort),
	server_(loop,listenAddr_,reusePort),
	threadNum_(0)
{
	newConnCb_ = nullptr;
	messageCb_ = defaultMessageCb;
	closeCb_ = nullptr;
	server_.setNewConnectionCallback(std::bind(&HttpServer::newConnCb_, this));
	server_.setMessageCallback(std::bind(&HttpServer::messageCb, this));
	server_.setCloseCallback(std::bind(&HttpServer::closeCb, this));
	server_.setThreadInitialCallback(std::bind(&HttpServer::threadInitial, this));
}

void ChatRoom::HttpServer::setThreadNumber(unsigned int num)
{
	threadNum_ = num;
	server_.setThreadNum(num);
}

void ChatRoom::HttpServer::start()
{
	server_.start();
}

void ChatRoom::HttpServer::newCb(TcpConnectionPtr ptr)
{
	//if (ptr->disConnected())
	printf("New connection %s created",ptr->getName());
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
	printf("Connection %s destroyed", ptr->getName());
	if (closeCb_) closeCb_(ptr);
}

void ChatRoom::HttpServer::threadInitial(EventLoop* loop)
{
	printf("New thread created");
	if (threadCb_) threadCb_(loop);
}
