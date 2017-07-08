#include "Connector.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"

#include <cassert>
using namespace ChatRoom;

Connector::Connector(EventLoop *loop, const struct sockaddr_in & server):loop_(loop),serverAddr_(server){
	connCb_ = nullptr;
	state_ = kDisConnected;
}

Connector:: ~Connector(){
	int sockfd = resetChannel();
	closeSocket(sockfd);
}

void Connector::start(){
	loop_->runInLoop(std::bind(&Connector::startInloop,this));
}

void Connector::stop(){
	loop_->runInLoop(std::bind(&Connector::stopInloop,this));
}

void Connector::startInloop(){
	{
		std::lock_guard<std::mutex> guard(mu_);
		if(state_ != kDisConnected) return;
		state_ = kConnecting;
	}
	int sockfd = createNonblockSocket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	int result = connectAddress(sockfd, sockaddr_cast(&serverAddr_), sizeof serverAddr_);
	if(result < 0){
		//Connect Failed
		//Tye again or not should depending on the error.
		closeSocket(sockfd);
		std::lock_guard<std::mutex> guard(mu_);
		state_ = kDisConnected;
		loop_->runInLoop(std::bind(&Connector::startInloop,this)); //Try again, add timer will be better
	}
	else{
		ch_ = new Channel(sockfd);
		ch_->setWriteCallback(std::bind(&Connector::handleWrite,this));
		loop_->updateChannle(ch_);
	}
}

void Connector::stopInloop(){
	{
		std::lock_guard<std::mutex> guard(mu_);
		if(state_ != kConnected) return;
		state_ = kDisConnecting;
	}
	int sockfd = resetChannel();
	closeSocket(sockfd);
	setState(States(kDisConnected));
}

int Connector::resetChannel(){
	int sockfd = ch_->getfd();
	ch_->disableAll();
	loop_->removeChannle(ch_);
	delete ch_;
	ch_ = nullptr;
	return sockfd;
}

void Connector::handleWrite(){
	int sockfd = resetChannel();
	assert(state_ == kDisConnecting);
	setState(States(kConnected));
	if(connCb_) connCb_(sockfd);
}