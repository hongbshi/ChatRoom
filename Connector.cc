#include "Connector.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"

#include <cassert>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <error.h>

using namespace ChatRoom;

Connector::Connector(EventLoop *loop, const struct sockaddr_in & server):loop_(loop),serverAddr_(server){
	connCb_ = nullptr;
	state_ = kDisConnected;
}

Connector:: ~Connector(){
	//can not close sockfd.
	//int sockfd = resetChannel();
	//closeSocket(sockfd);
}

void Connector::start(){
	loop_->runInLoop(std::bind(&Connector::startInloop,this));
}

void Connector::stop(){
	loop_->runInLoop(std::bind(&Connector::stopInloop,this));
}

void Connector::startInloop(){
	printf("File: Connector.cc, startInloop function.\n");
	{
		printf("File: Connector.cc, startInloop function, update Connect state.\n");
		std::lock_guard<std::mutex> guard(mu_);
		if(state_ != kDisConnected) return;
		state_ = kConnecting;
	}
	int sockfd = createNonblockSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	printf("File: Connector.cc, startInloop funtion, Connect sockfd is %d\n", sockfd);
	sockaddr *addr = sockaddr_cast(&serverAddr_);
	short port = ntohs(serverAddr_.sin_port);
	char ip[64];
	inet_ntop(AF_INET, (void*)(&serverAddr_.sin_addr), ip, sizeof ip);
	printf("File: Connector.cc, startInloop funtion, serverAddr_ is %s:%d.\n", ip, port);
	//printf("File: Connector.cc, startInloop funtion, serverAddr_.sin_port %d\n", port);
	int len = sizeof serverAddr_;
	//printf("File: Connector.cc, startInloop funtion, serverAddr_ length is %d\n", len);
	int result = connectAddress(sockfd, addr, len);

	if(result < 0 && errno != EINPROGRESS){
		printf("File: Connector.cc, startInloop function, Connect Failed, errno is %d.\n", errno);
		//Connect Failed
		//Tye again or not should depending on the error.
		closeSocket(sockfd);
		std::lock_guard<std::mutex> guard(mu_);
		state_ = kDisConnected;
		loop_->runInLoop(std::bind(&Connector::startInloop,this)); //Try again, add timer will be better
	}
	else{
		printf("File: Connector.cc, startInloop function, Connect First step Succeed.\n");
		ch_ = new Channel(sockfd);
		ch_->setWriteCallback(std::bind(&Connector::handleWrite,this));
		ch_->disableRead();
		ch_->enableWrite();
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
	printf("File: Connector.cc, resetChannel function.\n");
	int sockfd = ch_->getfd();
	ch_->disableAll();
	loop_->removeChannle(ch_);
	delete ch_;
	ch_ = nullptr;
	return sockfd;
}

void Connector::handleWrite(){
	printf("File: Connector.cc, handleWrite function start.\n");
	int sockfd = resetChannel();
	assert(state_ == kConnecting);
	setState(States(kConnected));
	//printf("File: Connector.cc, handleWrite function, connCb_ start\n");
	if(connCb_) connCb_(sockfd);
	//printf("File: Connector.cc, handleWrite function, connCb_ end\n");
	printf("File: Connector.cc, handleWrite function end.\n");
}
