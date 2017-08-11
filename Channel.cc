#include "Channel.h"

using namespace ChatRoom;

Channel::Channel(int fd)
{
	readCallback_ = nullptr;
	writeCallback_ = nullptr;
	errorCallback_ = nullptr;
	closeCallback_ = nullptr;
	Event_ = kNoneEvent;
	fd_ = fd;
	status_ = kNew;
	reEvent_ = kNoneEvent;
}

void Channel::handleEvent()
{
	printf("File: Channel.cc, Channel::handleEvent function, channel fd is %d.\n", fd_);
	//
	if(reEvent_ & EPOLLIN) printf("EPOLLIN\n");
	if(reEvent_ & EPOLLPRI) printf("EPOLLPRI\n");
	if(reEvent_ & EPOLLRDHUP) printf("EPOLLRDHUP\n");
	if(reEvent_ & EPOLLOUT) printf("EPOLLOUT\n");
	if(reEvent_ & EPOLLERR) printf("EPOLLERR\n");
	if(reEvent_ & EPOLLHUP) printf("EPOLLHUP\n");
	//
	if (reEvent_ == kNoneEvent)	return;
	if(!(reEvent_ & EPOLLIN) && (reEvent_ & EPOLLHUP) && closeCallback_){
		printf("File: Channel.cc, Channel::handleEvent function, close event.\n");
		closeCallback_();
	}
	if ((reEvent_ & EPOLLERR) && errorCallback_){
		printf("File: Channel.cc, Channel::handleEvent function, error event.\n");
		errorCallback_();
	}
	if ((reEvent_ & EPOLLOUT) && writeCallback_){
		printf("File: Channel.cc, Channel::handleEvent function, write event.\n");
		writeCallback_();
	}
	printf("***********************************\n");
	if(reEvent_ & EPOLLIN) printf("EPOLLIN\n");
	if(reEvent_ & EPOLLPRI) printf("EPOLLPRI\n");
	if(reEvent_ & EPOLLRDHUP) printf("EPOLLRDHUP\n");
	if(reEvent_ & EPOLLOUT) printf("EPOLLOUT\n");
	if(reEvent_ & EPOLLERR) printf("EPOLLERR\n");
	if(reEvent_ & EPOLLHUP) printf("EPOLLHUP\n");
	printf("***********************************\n");
	if ((reEvent_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) && readCallback_){
		printf("File: Channel.cc, Channel::handleEvent function, read event.\n");
		readCallback_();
	}
	/*
	if ((reEvent_ & EPOLLHUP) && !(reEvent_ & (EPOLLIN | EPOLLOUT)) && closeCallback_){
		printf("close call back, File: Channel.cc, Channel::handleEvent function.\n");
		closeCallback_();
	}
	*/
}

int const Channel::kNoneEvent = 0;
int const Channel::kReadEvent = (EPOLLIN | EPOLLPRI);
int const Channel::kWriteEvent = EPOLLOUT;


