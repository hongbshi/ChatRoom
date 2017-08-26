#include "TcpProxy.h"
#include "Socket.h"
#include "TcpProxyClient.h"
#include "TcpProxyServer.h"
#include "TcpConnection.h"
#include "Acceptor.h"
#include "ThreadPool.h"
#include "ReadAddr.h"
#include "ServerAddr.h"

#include <stdio.h>

using namespace ChatRoom;
using namespace std::placeholders;

//For Acceptor
void TcpProxy::newConnection(int sockfd, const struct sockaddr & clientAddr)
{
	printf("File: TcpProxy.cc, TcpProxy::newConnection function.\n");

	//
	char buff[32];
	EventLoop* loop= threadPool_->getNext();
	int number = ChatRoom::TcpProxyServer::getNumber();
	snprintf(buff, sizeof buff, "%s,%d", "TcpProxyServer", number);
	std::string name(buff);
	const struct sockaddr * laddr = sockaddr_cast(&listenAddr_);

	//Create TcpProxyServer
	TcpProxyServerPtr svr = std::make_shared<TcpProxyServer>(loop, sockfd, *laddr, clientAddr, name);
	svr->setCloseCallback(std::bind(&TcpProxy::closeServerCb, this, _1));
	server_.insert(std::make_pair(name, svr));

    //Create TcpProxyClient
	struct sockaddr_in serverAddr;
	if(!ServerAddr::getNext(serverAddr)){
		printf("File: TcpProxy, newConnection function, ServerAddr::getNext eror.\n");
		server_.erase(name);
		return;
	}
	number = ChatRoom::TcpProxyClient::getNumber();
	snprintf(buff, sizeof buff, "%s,%d", "TcpProxyClient", number);
	std::string cName(buff);
	TcpProxyClientPtr clt = std::make_shared<TcpProxyClient>(loop, serverAddr, cName);
	clt->setCloseCallback(std::bind(&TcpProxy::closeClientCb, this, _1));
	client_.insert(std::make_pair(cName, clt));

	//Connect TcpProxyServer and TcpProxyClient
	svr->setWeakContext(std::static_pointer_cast<void>(clt));
	clt->setWeakContext(std::static_pointer_cast<void>(svr));
	//clt->setState(ChatRoom::TcpProxyClient::State(kConnecting));

	//
	printf("File: TcpProxy.cc, TcpProxy::newConnection function end.\n");
}

//For TcpProxyClient
void TcpProxy::closeClientCb(TcpProxyClientPtr ptr){
	std::string name = ptr->getName();
	client_.erase(name);
	std::shared_ptr<void> tmp(ptr->getWeakContext().lock());
	if(tmp){
		TcpProxyServerPtr svr = std::static_pointer_cast<TcpProxyServer>(tmp);
		svr->shutdownWrite();
	}
}

//For TcpProxyServer
void TcpProxy::closeServerCb(TcpProxyServerPtr ptr){
	std::string name = ptr->getName();
	server_.erase(name);
	std::shared_ptr<void> tmp(ptr->getWeakContext().lock());
	if(tmp){
		TcpProxyClientPtr clt = std::static_pointer_cast<TcpProxyClient>(tmp);
		clt->shutdownWrite();
	}
}

TcpProxy::TcpProxy(EventLoop * loop, 
		const std::string & addrConf,
		bool reusePort):reusePort_(reusePort),
	loop_(loop),
	addrConf_(addrConf),
	threadNum_(0),
	threadInitialCallback_(nullptr)
{
}

void TcpProxy::start(){
	std::vector<struct sockaddr_in> addr;
	if(!ReadAddr::readServerAddr(addrConf_.c_str(), addr)){
		printf("File: TcpProxy, start function, read addrConf file error!\n");
		exit(-1);
	}
	if(addr.size() < 2){
		printf("File: TcpProxy, start function, we need two valid address at least.\n");
		exit(-1);
	}
	//Get Listen address
	listenAddr_ = addr.back();
	addr.pop_back();
	//
	ServerAddr::swapAddr(addr);
	//
	acceptor_ = std::make_shared<Acceptor>(loop_, &listenAddr_, reusePort_);
	acceptor_->setNewConnectCallback(std::bind(&TcpProxy::newConnection, this, _1, _2));
	threadPool_ = std::make_shared<ThreadPool>(loop_, threadInitialCallback_, threadNum_);
	threadPool_->start();
	acceptor_->listen();
	printf("File: TcpProxy.cc, TcpProxy::start function end.\n");
}
