#include "TcpProxyClient.h"
#include "TcpConnection.h"
#include "Socket.h"
#include "Connector.h"

using namespace ChatRoom;
using namespace std::placeholders;

TcpProxyClient::TcpProxyClient(EventLoop *loop, const struct sockaddr_in & server):loop_(loop),serverAddr_(server){
	isConnect_ = false;
	connector_ = std::make_shared<Connector>(loop,serverAddr_);
	connector_->setConnectionCallback(std::bind(&TcpProxyClient::connectorCb,this,_1));
	tcpConn_ = nullptr;
	context_ = nullptr;
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
	tcpConn_->close();
}

void TcpProxyClient::connectorCb(int sockfd){
	isConnect_ = true;
	const struct sockaddr local = getLocalAddr(sockfd);
	localAddr_ = *sockaddr_in_cast(&local);
	std::string name = "TcpClient connect";
	tcpConn_ = std::make_shared<TcpConnection>(loop_,sockfd,&localAddr_,&serverAddr_,name);
	tcpConn_->setConnectedCallback(std::bind(&TcpProxyClient::handleNewConnetion,this,_1));
	tcpConn_->setCloseCallback(std::bind(&TcpProxyClient::handleClose,this,_1));
	tcpConn_->setWriteCallback(std::bind(&TcpProxyClient::handleWrite,this,_1));
	tcpConn_->setMessageCallback(std::bind(&TcpProxyClient::handleMessage,this,_1,_2));
	//if(connCb_) connCb_(tcpConn_);
	loop_->runInLoop(std::bind(&TcpConnection::connectEstablished,tcpConn_));
}

//void TcpClient::handleWrite(TcpConnectionPtr ptr){
//	//log if necessary
//	if(writeCb_) writeCb_(ptr);
//}

void TcpProxyClient::handleNewConnetion(TcpConnectionPtr ptr){

}

void TcpProxyClient::handleMessage(TcpConnectionPtr ptr, Buffer* buff){
	outBuff_->writeToBuffer(buff);
	if(context_.lock() && context_->connected()) context_.send(outBuff_);
	else ptr->close();
}

void TcpProxyClient::handleClose(TcpConnectionPtr ptr){
	if(!isConnect_) return;
	loop_->runInLoop(std::bind(&TcpConnection::connectDestroyed,ptr));
	isConnect_ = false;
	tcpConn_ = nullptr;
}

void TcpProxyClient::handleWrite(TcpConnectionPtr ptr){
	ptr->send(inBuff_);
}

void send(Buffer *buff){
	inBuff_->writeToBuff(buff);
	if(isConnect_) tcpConn_->send(inBuff);
	else connect();
}
