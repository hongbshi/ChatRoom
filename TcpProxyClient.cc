#include "TcpProxyClient.h"
#include "TcpProxyServer.h"
#include "TcpConnection.h"
#include "Socket.h"
#include "Connector.h"

using namespace ChatRoom;
using namespace std::placeholders;

int TcpProxyClient::number_ = 1;

TcpProxyClient::TcpProxyClient(EventLoop *loop, 
		const struct sockaddr_in & server, 
		const string & name):name_(name), 
	loop_(loop),
	serverAddr_(server)
{
	connState_ = kDisconnected;
	connector_ = std::make_shared<Connector>(loop, serverAddr_);
	connector_->setConnectionCallback(std::bind(&TcpProxyClient::connectorCb, this, _1));
}

TcpProxyClient::~TcpProxyClient(){
	//log something
}

void TcpProxyClient::connect(){
	if(connState_ == kConnected) return;
	connector_->start();
}

void TcpProxyClient::disconnect(){
	if(connState_ == kDisconnected) return;
	loop_->runInLoop(std::bind(&TcpProxyClient::handleClose, this, tcpConn_));
}

void TcpProxyClient::shutdownWrite(){
	if(tcpConn_) tcpConn_->shutdownWrite();
}

void TcpProxyClient::connectorCb(int sockfd){
	setState(kConnected);
	const struct sockaddr local = getLocalAddr(sockfd);
	localAddr_ = *sockaddr_in_cast(&local);
	std::string name("TcpConnection");
	tcpConn_ = std::make_shared<TcpConnection>(loop_, sockfd, &localAddr_, &serverAddr_, name);
	tcpConn_->setConnectedCallback(std::bind(&TcpProxyClient::handleNewConnection, this, _1));
	tcpConn_->setCloseCallback(std::bind(&TcpProxyClient::handleClose, this, _1));
	//tcpConn_->setWriteCallback(std::bind(&TcpProxyClient::handleWrite,this,_1));
	tcpConn_->setMessageCallback(std::bind(&TcpProxyClient::handleMessage, this, _1, _2));
	loop_->runInLoop(std::bind(&TcpConnection::connectEstablished, tcpConn_));
}

void TcpProxyClient::handleNewConnection(TcpConnectionPtr ptr){
	//if(connCb_) connCb_(shared_from_this());
	if(!inBuff_.isEmpty()) ptr->send(inBuff_);
}

void TcpProxyClient::handleMessage(TcpConnectionPtr ptr, Buffer* buff){
	auto spt = weakContext_.lock();
	if(spt){
		TcpProxyServerPtr svr = std::static_pointer_cast<TcpProxyServer>(spt);
		svr->send(buff);
	}
	else handleClose(ptr);
}

void TcpProxyClient::handleClose(TcpConnectionPtr ptr){
	if(connState_ == kDisconnected) return;
	connState_ = kDisconnected;
	loop_->runInLoop(std::bind(&TcpConnection::connectDestroyed, ptr));
	std::shared_ptr<TcpProxyClient> guard(shared_from_this());
	if(closeCb_) closeCb_(guard);
}

void TcpProxyClient::handleWrite(TcpConnectionPtr ptr){
	//if(inBuff_.isEmpty()) ptr->stopWrite();
	//else ptr->send(inBuff_);
}

void TcpProxyClient::handleReadZero(TcpConnectionPtr ptr){
	auto spt = weakContext_.lock();
	if(spt){
		TcpProxyServerPtr svr = std::static_pointer_cast<TcpProxyServer>(spt);
		svr->shutdownWrite();
	}
	else ptr->close();
}

void TcpProxyClient::send(Buffer *buff){
	inBuff_.writeToBuffer(*buff);
	if(connState_ == kConnected) tcpConn_->send(inBuff_);
	else connect();
}
