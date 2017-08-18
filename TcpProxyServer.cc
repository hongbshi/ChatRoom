#include "TcpProxyServer.h"
#include "Socket.h"
#include "TcpProxyClient.h"
#include "TcpConnection.h"
#include "Acceptor.h"
#include "ThreadPool.h"
#include "ReadAddr.h"
#include "ServerAddr.h"

#include <stdio.h>

using namespace ChatRoom;
using namespace std::placeholders;
//For Acceptor
void TcpProxyServer::newConnection(int sockfd, const struct sockaddr & clientAddr)
{
	printf("File: TcpProxyServer.cc, TcpProxyServer::newConnection function.\n");
	char buff[32];
	int number = ChatRoom::TcpConnection::getNumber();
	snprintf(buff, sizeof buff, "%s,%d", "TcpConnection", number);
	std::string name(buff);
	EventLoop* loop= threadPool_->getNext();
	const struct sockaddr_in * addr = sockaddr_in_cast(&clientAddr);
	//Create TcpConnetion
	std::shared_ptr<TcpConnection> conn = std::make_shared<TcpConnection>(loop, 
			sockfd, 
			&listenAddr_, 
			addr,
			name);
	conn->setConnectedCallback(std::bind(&TcpProxyServer::handleNewConnection,this,_1));
	conn->setMessageCallback(std::bind(&TcpProxyServer::handleMessage,this,_1,_2));
	conn->setCloseCallback(std::bind(&TcpProxyServer::removeConnection,this,_1));
	conn_[name] = conn;
    //Create TcpProxyClient
	struct sockaddr_in serverAddr;
	if(!ServerAddr::getNext(serverAddr)){
		printf("File: TcpProxyServer, newConnection function, ServerAddr::getNext eror.\n");
		conn_.erase(name);
		return;
	}
	number = ChatRoom::TcpProxyClient::getNum();
	snprintf(buff, sizeof buff, "%s,%d", "TcpProxyClient", number);
	std::string cName(buff);
	std::shared_ptr<TcpProxyClient> client = std::make_shared<TcpProxyClient>(loop, serverAddr, cName);
	client->setNewConnectionCallback(std::bind(&TcpProxyServer::newClientCb, this, _1));
	//client->setWriteCallback(std::bind(&TcpProxyServer::handleWrite,this,_1));
	//client->setMessageCallback(std::bind(&TcpProxyServer::handleMessage,this,_1,_2));
	client->setCloseCallback(std::bind(&TcpProxyServer::closeClientCb, this, _1));
	//Connect TcpConnection and TcpProxyClient
	conn->setWeakContext(std::static_pointer_cast<void>(client));
	client->setWeakContext(conn);
	client_.insert(std::make_pair(cName, client));
    //	
	loop->runInLoop(std::bind(&TcpConnection::connectEstablished,conn));
	//loop->runInLoop(std::bind(&TcpProxyClient::connect,client));
	printf("File: TcpProxyServer.cc, TcpProxyServer::newConnection function end.\n");
}

void TcpProxyServer::newClientCb(TcpProxyClientPtr ptr){
	//std::string name = ptr->getName();
	//client_.insert(std::make_pair(name, ptr));
}

void TcpProxyServer::closeClientCb(TcpProxyClientPtr ptr){
	std::string name = ptr->getName();
	client_.erase(name);
	TcpConnectionPtr tmp(ptr->getWeakContext().lock());
	if(tmp && tmp->connected()){
		EventLoop* ioloop = tmp->getLoop();
		ioloop->runInLoop(std::bind(&TcpConnection::close, tmp));	
	}
}

void TcpProxyServer::removeConnection(TcpConnectionPtr ptr)
{
	loop_->runInLoop(std::bind(&TcpProxyServer::removeConnectionInLoop, this, ptr));
}

void TcpProxyServer::removeConnectionInLoop(TcpConnectionPtr ptr)
{
	printf("File: TcpProxyServer.cc, removeConnectionInLoop function.\n");
	conn_.erase(ptr->getName());
	EventLoop* ioloop = ptr->getLoop();
	ioloop->runInLoop(std::bind(&TcpConnection::connectDestroyed, ptr));
	std::shared_ptr<void> client(ptr->getWeakContext().lock());
	if(client){
		std::shared_ptr<TcpProxyClient> tmp = std::static_pointer_cast<TcpProxyClient>(client);
		if(tmp->isconnect()) tmp->disconnect();
	}
}

TcpProxyServer::TcpProxyServer(EventLoop * loop, 
		const std::string & addrConf,
		bool reusePort)
	:loop_(loop),
	addrConf_(addrConf),
	reusePort_(reusePort),
	threadNum_(0),
	threadInitialCallback_(),
	threadPool_(nullptr){}

void TcpProxyServer::setThreadNum(const unsigned int num){
	threadNum_ = num;
}

void TcpProxyServer::setThreadInitialCallback(ThreadInitialCallback & initial){
	threadInitialCallback_ = initial;
}

void TcpProxyServer::setThreadInitialCallback(ThreadInitialCallback && initial){
	threadInitialCallback_ = std::move(initial);
}

void TcpProxyServer::setNewConnectionCallback(NewConnectionCallback & cb){
	newConnectionCallback_ = cb;
}

void TcpProxyServer::setNewConnectionCallback(NewConnectionCallback && cb){
	newConnectionCallback_ = std::move(cb);
}

void TcpProxyServer::setMessageCallback(MessageCallback & cb){
	messageCallback_ = cb;
}

void TcpProxyServer::setMessageCallback(MessageCallback && cb){
	messageCallback_ = std::move(cb);
}

void TcpProxyServer::setCloseCallback(CloseCallback & cb){
	closeCallback_ = cb;
}

void TcpProxyServer::setCloseCallback(CloseCallback && cb){
	closeCallback_ = std::move(cb);
}

void TcpProxyServer::start(){
	std::vector<struct sockaddr_in> addr;
	if(!ReadAddr::readServerAddr(addrConf_.c_str(), addr)){
		printf("File: TcpProxyServer, start function, read addrConf file error!\n");
		exit(-1);
	}
	if(addr.size() < 2){
		printf("File: TcpProxyServer, start function, we need two valid address at least.\n");
		exit(-1);
	}
	//Get Listen address
	listenAddr_ = addr.back();
	addr.pop_back();
	//
	ServerAddr::swapAddr(addr);
	//
	acceptor_ = std::make_shared<Acceptor>(loop_, &listenAddr_, reusePort_);
	acceptor_-> setNewConnectCallback(std::bind(&TcpProxyServer::newConnection, this, _1, _2));
	threadPool_ = std::make_shared<ThreadPool>(loop_, threadInitialCallback_, threadNum_);
	threadPool_->start();
	acceptor_->listen();
	printf("File: TcpProxyServer.cc, TcpProxyServer::start function end.\n");
}

void TcpProxyServer::handleNewConnection(TcpConnectionPtr ptr){
	//
}

void TcpProxyServer::handleMessage(TcpConnectionPtr ptr, Buffer *buff){
	EventLoop *ioLoop = ptr->getLoop();
	std::shared_ptr<void> tmp(ptr->getWeakContext().lock());
	if(tmp){
	    std::shared_ptr<TcpProxyClient> client = std::static_pointer_cast<TcpProxyClient>(tmp);
		ioLoop->runInLoop(std::bind(&TcpProxyClient::send, client, buff));
	}
	else{
		ptr->close();
	}
}

