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
	printf("Channel handleEvent, File: Channel.cc, Channel::handleEvent function.\n");
	if (reEvent_ == kNoneEvent)
		return;
	if ((reEvent_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) && readCallback_){
		printf("read call back, File: Channel.cc, Channel::handleEvent function.\n");
		readCallback_();
	}
	if ((reEvent_ & EPOLLOUT) && writeCallback_){
		printf("write call back, File: Channel.cc, Channel::handleEvent function.\n");
		writeCallback_();
	}
	if ((reEvent_ & EPOLLERR) && errorCallback_){
		printf("error call back, File: Channel.cc, Channel::handleEvent function.\n");
		errorCallback_();
	}
	if ((reEvent_ & EPOLLHUP) && closeCallback_){
		printf("close call back, File: Channel.cc, Channel::handleEvent function.\n");
		closeCallback_();
	}
}

int const Channel::kNoneEvent = 0;
int const Channel::kReadEvent = EPOLLIN | EPOLLPRI;
int const Channel::kWriteEvent = EPOLLOUT;


