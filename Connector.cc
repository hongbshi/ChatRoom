#include "Connector.h"
#include "Socket.h"
#include "EventLoop.h"
using namespace ChatRoom;

Connector::Connector(EventLoop *loop, const struct sockaddr_in & server):loop_(loop),serverAddr_(server){
	connCb_ = nullptr;
	state_ = kDisConnected;



}

void Connector::start(){
	loop_->runInLoop(&Connector::startInloop,this);
}

void Connector::stop(){
	loop_->runInLoop(&Connector::stopInloop,this);
}

void Connector::startInloop(){
	int result = connectAddress(sockfd, *addr, length);
}

void Connector::stopInloop(){

}

