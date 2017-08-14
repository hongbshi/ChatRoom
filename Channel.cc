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
	tie_ = false;
}

void Channel::handleEvent(){
	if(tie_){
		std::shared_ptr<void> guard = context_.lock();
		if(guard) handleEventWithGuard();
	}
	else handleEventWithGuard();
}

void Channel::handleEventWithGuard()
{
	printf("File: Channel.cc, Channel::handleEventWithGuard function, channel fd is %d.\n", fd_);
	//
	//if(reEvent_ & EPOLLIN) printf("EPOLLIN\n");
	//if(reEvent_ & EPOLLPRI) printf("EPOLLPRI\n");
	//if(reEvent_ & EPOLLRDHUP) printf("EPOLLRDHUP\n");
	//if(reEvent_ & EPOLLOUT) printf("EPOLLOUT\n");
	//if(reEvent_ & EPOLLERR) printf("EPOLLERR\n");
	//if(reEvent_ & EPOLLHUP) printf("EPOLLHUP\n");
	//
	int ev = reEvent_;
	if (ev == kNoneEvent) return;
	if(!(ev & EPOLLIN) && (ev & EPOLLHUP) && closeCallback_){
		printf("File: Channel.cc, Channel::handleEventWithGuard function, close event.\n");
		closeCallback_();
	}
	if ((ev & EPOLLERR) && errorCallback_){
		printf("File: Channel.cc, Channel::handleEventWithGuard function, error event.\n");
		errorCallback_();
	}
	if ((ev & EPOLLOUT) && writeCallback_){
		printf("File: Channel.cc, Channel::handleEventWithGuard function, write event.\n");
		writeCallback_();
	}
	if ((ev & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) && readCallback_){
		printf("File: Channel.cc, Channel::handleEventWithGuard function, read event.\n");
		readCallback_();
	}
}

int const Channel::kNoneEvent = 0;
int const Channel::kReadEvent = (EPOLLIN | EPOLLPRI);
int const Channel::kWriteEvent = EPOLLOUT;


