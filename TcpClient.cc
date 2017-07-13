#include "TcpClient.h"
#include "Socket.h"
#include "Connector.h"

using namespace ChatRoom;

TcpClient::TcpClient(EventLoop *loop, const struct sockaddr_in & server):loop_(loop),serverAddr_(server){
	isConnect_ = false;
	connector_ = std::make_shared<Connector>(loop,serverAddr_);
	connector_->set
	tcpConn_ = nullptr;
}

TcpClient::~TcpClient(){

}

void TcpClient::connect(){

}

void TcpClient::disconnect(){

}

void TcpClient::connectorCb(int sockfd){
	localAddr_ = *sockaddr_in_cast(&getLocalAddr(sockfd));
	std::string name = "TcpClient connect";
	tcpConn_ = std::make_shared<TcpConnecion>(loop_,sockfd,localAddr_,serverAddr_,name);
	if(connCb_) connCb_(tcpConn_);
}
