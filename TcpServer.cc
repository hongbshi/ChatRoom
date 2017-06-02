#include "Socket.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "Acceptor.h"
#include "ThreadPool.h"
#include <stdio.h>

using namespace ChatRoom;
using namespace std::placeholders;
void ChatRoom::TcpServer::newConnection(int sockfd, const struct sockaddr & clientAddr)
{
	char buff[32];
	int number = ChatRoom::TcpConnection::getNumber();
	snprintf(buff,sizeof buff, "%s,%d","TcpConnection", number);
	std::string name(buff);
	std::shared_ptr<TcpConnection> conn =
		std::make_shared<TcpConnection>(loop_, 
			sockfd, 
			&listenAddr_, 
			sockaddr_in_cast(&clientAddr),
			name);
	conn->setConnectedCallback(newConnectionCallback_);
	conn->setMessageCallback(messageCallback_);
	//conn->setWriteCallback(writeCompleteCallback_);
	conn->setCloseCallback(std::bind(&TcpServer::removeConnection,this,_1));
	conn_[name] = conn;
	EventLoop* loop= threadPool_->getNext();
	loop->runInLoop(std::bind(&TcpConnection::connectEstablished,conn));
}

void ChatRoom::TcpServer::removeConnection(TcpConnectionPtr ptr)
{
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, ptr));
}

void ChatRoom::TcpServer::removeConnectionInLoop(TcpConnectionPtr ptr)
{
	conn_.erase(ptr->getName());
	EventLoop* ioloop = ptr->getLoop();
	ioloop->runInLoop(std::bind(&TcpConnection::connectDestroyed, ptr));
}

ChatRoom::TcpServer::TcpServer(EventLoop * loop, 
	const struct sockaddr_in * listenAddr, bool reusePort)
	:loop_(loop),listenAddr_(*listenAddr),reusePort_(reusePort),
	threadNum_(0),threadInitialCallback_()
{
	acceptor_ = std::make_shared<Acceptor>(loop, listenAddr, reusePort);
	acceptor_-> setNewConnectCallback(std::bind(&TcpServer::newConnection, this,_1,_2));
	threadPool_ = nullptr;
}

void ChatRoom::TcpServer::setThreadNum(const unsigned int num)
{
	threadNum_ = num;
}

void ChatRoom::TcpServer::setThreadInitialCallback(ThreadInitialCallback & initial)
{
	threadInitialCallback_ = initial;
}

void ChatRoom::TcpServer::setThreadInitialCallback(ThreadInitialCallback && initial)
{
	threadInitialCallback_ = std::move(initial);
}

void ChatRoom::TcpServer::setNewConnectionCallback(NewConnectionCallback & cb)
{
	newConnectionCallback_ = cb;
}

void ChatRoom::TcpServer::setNewConnectionCallback(NewConnectionCallback && cb)
{
	newConnectionCallback_ = std::move(cb);
}

void ChatRoom::TcpServer::setMessageCallback(MessageCallback & cb)
{
	messageCallback_ = cb;
}

void ChatRoom::TcpServer::setMessageCallback(MessageCallback && cb)
{
	messageCallback_ = std::move(cb);
}

//void ChatRoom::TcpServer::setWriteCompleteCallback(WriteCompleteCallback &&cb)
//{
//	writeCompleteCallback_ = cb;
//}

void ChatRoom::TcpServer::setCloseCallback(CloseCallback & cb)
{
	closeCallback_ = cb;
}

void ChatRoom::TcpServer::setCloseCallback(CloseCallback &&cb)
{
	closeCallback_ = std::move(cb);
}

void ChatRoom::TcpServer::start()
{
	threadPool_ = std::make_shared<ThreadPool>(loop_,threadInitialCallback_, threadNum_);
	threadPool_->start();
	acceptor_->listen();
	printf("%s\n", "TcpServer start ok! TcpServer.cc, TcpServer::start function");
}
