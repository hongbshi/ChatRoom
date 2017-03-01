#include "TcpServer.h"
#include "TcpConnection.h"
#include "Acceptor.h"
#include "ThreadPool.h"
#include<stdio.h>

void ChatRoom::TcpServer::newConnection(int sockfd, sockaddr * clientAddr)
{
	char buff[30];
	int number = ChatRoom::TcpConnection::getNumber();
	snprintf(buff,sizeof buff, "%s,%d","TcpConnection", number);
	std::string name(buff);
	std::shared_ptr<TcpConnection> conn =
		std::make_shared<TcpConnection>(loop_, sockfd, listenAddr_, clientAddr,
			name);
	conn->setConnectedCallback(newConnectionCallback_);
	conn->setMessageCallback(messageCallback_);
	//conn->setWriteCallback(writeCompleteCallback_);
	conn->setCloseCallback(std::bind(&TcpServer::removeConnection,this));
	//¼ÇÂ¼
	conn_[name] = conn;
	EventLoop* loop= threadPool_->getNext();
	loop->runInLoop(std::bind(&TcpConnection::connectEstablished,this));
}

void ChatRoom::TcpServer::removeConnection(TcpConnectionPtr ptr)
{
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this));
}

void ChatRoom::TcpServer::removeConnectionInLoop(TcpConnectionPtr ptr)
{
	//É¾³ý¼ÇÂ¼
	conn_.erase(ptr->getName());
	EventLoop* ioloop = ptr->getLoop();
	ioloop->runInLoop(std::bind(&TcpConnection::connectDestroyed, ptr));
}

ChatRoom::TcpServer::TcpServer(EventLoop * loop, 
	const sockaddr * listenAddr, bool reusePort)
	:loop_(loop),listenAddr_(*listenAddr),reusePort_(reusePort),
	threadNum_(0),threadInitialCallback_()
{
	acceptor_ = std::make_shared<Acceptor>(loop,listenAddr,reusePort);
	acceptor_->setNewConnectCallback(std::bind(&TcpServer::newConnection, this));
	threadPool_ = nullptr;
}

void ChatRoom::TcpServer::setThreadNum(const unsigned int num)
{
	threadNum_ = num;
}

void ChatRoom::TcpServer::setThreadInitialCallback(Functor&& initial)
{
	threadInitialCallback_ = initial;
}

void ChatRoom::TcpServer::setNewConnectionCallback(NewConnectionCallback && cb)
{
	newConnectionCallback_ = cb;
}

void ChatRoom::TcpServer::setMessageCallback(MessageCallback &&cb)
{
	messageCallback_ = cb;
}

//void ChatRoom::TcpServer::setWriteCompleteCallback(WriteCompleteCallback &&cb)
//{
//	writeCompleteCallback_ = cb;
//}

void ChatRoom::TcpServer::setCloseCallback(CloseCallback &&cb)
{
	closeCallback_ = cb;
}

void ChatRoom::TcpServer::start()
{
	threadPool_ = std::make_shared<ThreadPool>(loop_,threadInitialCallback_, threadNum_);
	threadPool_->start();
	acceptor_->listen();
}
