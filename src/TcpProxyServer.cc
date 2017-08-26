#include "TcpProxyServer.h"
#include "Socket.h"
#include "TcpProxyClient.h"
#include "TcpConnection.h"
#include "Acceptor.h"
#include "ThreadPool.h"
#include "ReadAddr.h"
#include "ServerAddr.h"
#include "Buffer.h"

#include <stdio.h>

using namespace ChatRoom;
using namespace std::placeholders;

int TcpProxyServer::number_ = 1;

TcpProxyServer::TcpProxyServer(EventLoop * loop, 
		int sockfd, 
		const struct sockaddr & localAddr,
		const struct sockaddr & peerAddr,
		const std::string & name):loop_(loop),
	name_(name),
	closeCb_(nullptr)
{
	printf("File: TcpProxyServer.cc, TcpProxyServer::TcpProxyServer function.\n");
	//
	std::string cName("TcpConnection");
	const struct sockaddr_in * paddr = sockaddr_in_cast(&peerAddr);
	const struct sockaddr_in * laddr = sockaddr_in_cast(&localAddr);
	//Create TcpConnetion
	conn_ = std::make_shared<TcpConnection>(loop, sockfd, laddr, paddr, cName);
	//conn_->setConnectedCallback(std::bind(&TcpProxyServer::handleNewConnection,this,_1));
	conn_->setMessageCallback(std::bind(&TcpProxyServer::handleMessage, this, _1, _2));
	conn_->setCloseCallback(std::bind(&TcpProxyServer::removeConnection, this, _1));
	loop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn_));
	//
	printf("File: TcpProxyServer.cc, TcpProxyServer::newConnection function end.\n");
}

TcpProxyServer::~TcpProxyServer(){
	if(conn_ && conn_->connected()) loop_->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn_));	
}

void TcpProxyServer::removeConnection(TcpConnectionPtr ptr)
{
	loop_->runInLoop(std::bind(&TcpProxyServer::removeConnectionInLoop, this, ptr));
}

void TcpProxyServer::removeConnectionInLoop(TcpConnectionPtr ptr)
{
	printf("File: TcpProxyServer.cc, removeConnectionInLoop function.\n");
	EventLoop* ioloop = ptr->getLoop();
	ioloop->runInLoop(std::bind(&TcpConnection::connectDestroyed, ptr));
	TcpProxyServerPtr guard(shared_from_this());
	if(closeCb_) closeCb_(guard);
}

void TcpProxyServer::shutdownWrite(){
	conn_->shutdownWrite();
}

void TcpProxyServer::handleNewConnection(TcpConnectionPtr ptr){
	//
}

void TcpProxyServer::handleMessage(TcpConnectionPtr ptr, Buffer *buff){
	std::shared_ptr<void> tmp(weakContext_.lock());
	if(tmp){
		TcpProxyClientPtr clt = std::static_pointer_cast<TcpProxyClient>(tmp);
		clt->send(buff);
	}
	else removeConnection(ptr);
}

void TcpProxyServer::handleReadZero(TcpConnectionPtr ptr){
	auto tmp(weakContext_.lock());
	if(tmp){
		TcpProxyClientPtr clt = std::static_pointer_cast<TcpProxyClient>(tmp);
		clt->shutdownWrite();
	}
	else removeConnection(ptr);
}

void TcpProxyServer::send(Buffer *buff){
	conn_->send(*buff);
}

