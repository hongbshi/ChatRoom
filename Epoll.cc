#include "Epoll.h"
#include<assert.h>
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
	int number=epoll_wait(epollfd_, &event_, size, timeout);
	if (0 < number)
	{
		fillActiveChannel(number,activeChannel);
		if (number == size)
			event_.resize(2 * size);
	}
}
 void Epoll::updateChannel(Channel* ch)
 {
	 int status = ch->getStatus();
	 int fd = ch->getfd();
	 if (status==kNew || status==kDeleted)
	 {
		 if (status == kNew)
			 activeChannel_[fd] = ch;
		 ch->setStatus(kAdded);
		 update(EPOLL_CTL_ADD, ch);
			
	 }
	 else
	 {
		 if (ch->isNoneEvent())
		 {
			 update(EPOLL_CTL_DEL, ch);
			 ch->setStatus(kDeleted);
		 }
		 else
			update(EPOLL_CTL_MOD, ch);
	 }
 }

 void Epoll::removeChannel(Channel* ch)
 {
	 int status = ch->getStatus();
	 int fd = ch->getfd();
	 assert(status == kAdded || status==kDeleted);
	 if (status == kAdded)
		 update(EPOLL_CTL_DEL, ch);
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
	memset(&event, sizeof(event));
	event.events = ch->getEvent();
	event.data.ptr = ch;
	if (epoll_ctl(epollfd_, operation, ch->getfd(), event) < 0)
		abort();
}

void Epoll::fillActiveChannel(int number, std::vector<Channel*>& activeChannel)
{

}
