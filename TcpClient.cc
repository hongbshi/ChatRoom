#include "TcpClient.h"
#include "TcpConnection.h"
#include "Socket.h"
#include "Connector.h"

using namespace ChatRoom;
using namespace std::placeholders;

TcpClient::TcpClient(EventLoop *loop, const struct sockaddr_in & server):loop_(loop),serverAddr_(server){
	isConnect_ = false;
	connector_ = std::make_shared<Connector>(loop,serverAddr_);
	connector_->setConnectionCallback(std::bind(&TcpClient::connectorCb,this,_1));
	tcpConn_ = nullptr;
}

TcpClient::~TcpClient(){
	//log something
}

void TcpClient::connect(){
	if(isConnect_) return;
	connector_->start();
}

void TcpClient::disconnect(){
	if(!isConnect_) return;
	tcpConn_->close();
}

void TcpClient::connectorCb(int sockfd){
	isConnect_ = true;
	const struct sockaddr local = getLocalAddr(sockfd);
	localAddr_ = *sockaddr_in_cast(&local);
	std::string name = "TcpClient connect";
	tcpConn_ = std::make_shared<TcpConnection>(loop_,sockfd,&localAddr_,&serverAddr_,name);
	tcpConn_->setConnectedCallback(connCb_);
	tcpConn_->setCloseCallback(std::bind(&TcpClient::handleClose,this,_1));
	tcpConn_->setWriteCallback(std::bind(&TcpClient::handleWrite,this,_1));
	tcpConn_->setMessageCallback(std::bind(&TcpClient::handleMessage,this,_1,_2));
	//if(connCb_) connCb_(tcpConn_);
	loop_->runInLoop(std::bind(&TcpConnection::connectEstablished,tcpConn_));
}

void TcpClient::handleWrite(TcpConnectionPtr ptr){
	//log if necessary
	if(writeCb_) writeCb_(ptr);
}

void TcpClient::handleMessage(TcpConnectionPtr ptr, Buffer* buff){
	//Deal Message if necessary
	if(messCb_) messCb_(ptr,buff);
}

void TcpClient::handleClose(TcpConnectionPtr ptr){
	if(!isConnect_) return;
	if(closeCb_) closeCb_(ptr);
	loop_->runInLoop(std::bind(&TcpConnection::connectDestroyed,ptr));
	isConnect_ = false;
	tcpConn_ = nullptr;
}
