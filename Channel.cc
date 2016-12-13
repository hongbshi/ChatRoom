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

}

int const  Channel::kNoneEvent = 0;
int const Channel::kReadEvent = EPOLLIN| EPOLLPRI;
int const Channel::kWriteEvent = EPOLLOUT;

