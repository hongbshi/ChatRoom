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
	if (reEvent_ == kNoneEvent)
		return;
	if ((reEvent_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) && readCallback_)
		readCallback_();
	if ((reEvent_ & EPOLLOUT) && writeCallback_)
		writeCallback_();
	if ((reEvent_ & EPOLLERR) && errorCallback_)
		errorCallback_();
	if ((reEvent_ & EPOLLHUP) && closeCallback_)
		closeCallback_();
}

int const  Channel::kNoneEvent = 0;
int const Channel::kReadEvent = EPOLLIN | EPOLLPRI;
int const Channel::kWriteEvent = EPOLLOUT;


