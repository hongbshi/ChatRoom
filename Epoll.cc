#include "Epoll.h"
#include "Channel.h"
#include <assert.h>
#include <string.h>
using namespace ChatRoom;

Epoll::Epoll()
{
	epollfd_ = epoll_create(Epoll_Listen_Size);
	initialSize = 16;
	event_.resize(initialSize);
}

void Epoll::poll(int timeout,std::vector<Channel*>& activeChannel)
{
	int size = event_.size();
	int number = epoll_wait(epollfd_, &*event_.begin(), size, timeout);
	//printf("File: Epoll.cc, poll function, Current listen fd size is %d\n",(int)activeChannel_.size());
	printf("File: Epoll.cc, poll function. Epoll event size is %d.\n", number);
	if (number > 0) 
	{
		fillActiveChannel(number, activeChannel);
		if (number == size && size < Epoll_Listen_Size) event_.resize(2 * size);
	}
}

void Epoll::updateChannel(Channel* ch)
{
	 int status = ch->getStatus();
	 int fd = ch->getfd();
	 if (status == kNew || status == kDeleted)
	 {
		 if (status == kNew)
			 activeChannel_[fd] = ch;
		 ch->setStatus(kAdded);
		 //printf("File: Epoll.cc, updateChannel function, Add listen fd %d.\n",ch->getfd());
		 update(EPOLL_CTL_ADD, ch);
			
	 }
	 else
	 {
		 if (ch->isNoneEvent())
		 {
			 //printf("File: Epoll.cc, updateChannel function, Delete listen fd %d.\n",ch->getfd());
			 update(EPOLL_CTL_DEL, ch);
			 ch->setStatus(kDeleted);
		 }
		 else{
			 //printf("File: Epoll.cc, updateChannel function, Modify  listen fd %d.\n",ch->getfd());
			 update(EPOLL_CTL_MOD, ch);
		 }
	 }
}

void Epoll::removeChannel(Channel* ch)
{
	 printf("File: Epoll.cc, removeChannel function start.\n");
	 int status = ch->getStatus();
	 int fd = ch->getfd();
	 assert(status == kAdded || status == kDeleted);
	 if (status == kAdded){
		 //printf("File: Epoll,cc, removeChannel function, Delete listen fd is %d. \n",fd);
		 update(EPOLL_CTL_DEL, ch);
		 //ch->setStatus(kDeleted);
	 }
	 auto it = activeChannel_.find(fd);
	 activeChannel_.erase(it);
	 ch->setStatus(kNew);
}

bool Epoll::hasChannel(Channel* ch)
{
	return !(ch->getStatus() == kNew);
}

void Epoll::update(int operation,Channel* ch)
{
	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = ch->getEvent();
	event.data.ptr = ch;
	//printf("File: Epoll.cc, update function, events is %u.\n",event.events);
	if (epoll_ctl(epollfd_, operation, ch->getfd(), &event) < 0){
		printf("File: Epoll.cc, update function, epoll_ctl error.\n");
		abort();
	}
}

void Epoll::fillActiveChannel(int number, std::vector<Channel*>& activeChannel)
{
	activeChannel.reserve(number);
	for (int i = 0; i < number; i++)
	{
		Channel* tmp =static_cast<Channel*>(event_[i].data.ptr);
		tmp->setReEvent(event_[i].events);
		activeChannel.push_back(tmp);
	}
	printf("File: Epoll.cc, Epoll::fillActiveChannel function end.\n");
}
