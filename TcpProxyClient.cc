#include "TcpProxyClient.h"
#include "TcpConnection.h"
#include "Socket.h"
#include "Connector.h"

using namespace ChatRoom;
using namespace std::placeholders;

int TcpProxyClient::num_ = 0;

TcpProxyClient::TcpProxyClient(EventLoop *loop, const struct sockaddr_in & server, const string & name)
	:loop_(loop), serverAddr_(server), name_(name){
	isConnect_ = false;
	connector_ = std::make_shared<Connector>(loop, serverAddr_);
	connector_->setConnectionCallback(std::bind(&TcpProxyClient::connectorCb, this, _1));
	tcpConn_ = nullptr;
}

TcpProxyClient::~TcpProxyClient(){
	//log something
}

void TcpProxyClient::connect(){
	if(isConnect_) return;
	connector_->start();
}

void TcpProxyClient::disconnect(){
	if(!isConnect_) return;
	loop_->runInLoop(std::bind(&TcpProxyClient::handleClose, this, tcpConn_));
}

void TcpProxyClient::connectorCb(int sockfd){
	isConnect_ = true;
	const struct sockaddr local = getLocalAddr(sockfd);
	localAddr_ = *sockaddr_in_cast(&local);
	std::string name = "TcpProxyClient connect";
	tcpConn_ = std::make_shared<TcpConnection>(loop_, sockfd, &localAddr_, &serverAddr_, name);
	tcpConn_->setConnectedCallback(std::bind(&TcpProxyClient::handleNewConnection,this,_1));
	tcpConn_->setCloseCallback(std::bind(&TcpProxyClient::handleClose,this,_1));
	tcpConn_->setWriteCallback(std::bind(&TcpProxyClient::handleWrite,this,_1));
	tcpConn_->setMessageCallback(std::bind(&TcpProxyClient::handleMessage,this,_1,_2));
	loop_->runInLoop(std::bind(&TcpConnection::connectEstablished, tcpConn_));
}

void TcpProxyClient::handleNewConnection(TcpConnectionPtr ptr){
	if(connCb_) connCb_(shared_from_this());
	if(!inBuff_.isEmpty()) ptr->send(inBuff_);
}

void TcpProxyClient::handleMessage(TcpConnectionPtr ptr, Buffer* buff){
	outBuff_.writeToBuffer(*buff);
	auto spt = weakContext_.lock();
	if(spt && spt->connected()) spt->send(outBuff_);
	else handleClose(ptr);
}

void TcpProxyClient::handleClose(TcpConnectionPtr ptr){
	if(!isConnect_) return;
	isConnect_ = false;
	std::shared_ptr<TcpProxyClient> guard(shared_from_this());
	loop_->runInLoop(std::bind(&TcpConnection::connectDestroyed, ptr));
	if(closeCb_) closeCb_(guard);
	tcpConn_ = nullptr;
}

void TcpProxyClient::handleWrite(TcpConnectionPtr ptr){
	if(inBuff_.isEmpty()) ptr->stopWrite();
	else ptr->send(inBuff_);
}

void TcpProxyClient::send(Buffer *buff){
	inBuff_.writeToBuffer(*buff);
	if(isConnect_) tcpConn_->send(inBuff_);
	else connect();
}
