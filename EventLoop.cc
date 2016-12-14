#include "EventLoop.h"
#include "Epoll.h"
#include "MutexLockGuard.h"
#include "Channel.h"
#include "Thread.h"
#include<sys/eventfd.h>
#include<memory>
using namespace ChatRoom;

EventLoop::EventLoop()
{
	kEpollTimeoutMs_ = 10000;
	tid_ = getCurrentThreadTid();
	looping_ = false;
	quit_ = false;
	wakeupfd_ = eventfd(0, 0);
	wakeupChannel_=std::make_shared<Channel>(wakeupfd_);
	wakeupChannel_->enableRead();
	epoll_ = std::make_shared<Epoll>();
	epoll_->updateChannel(&*wakeupChannel_);
}

void ChatRoom::EventLoop::loop()
{

}

void ChatRoom::EventLoop::wakeup()
{
	
}

void ChatRoom::EventLoop::removeChannle(Channel * ch)
{
	epoll_->removeChannel(ch);
}

void ChatRoom::EventLoop::updateChannle(Channel * ch)
{
	epoll_->updateChannel(ch);
}

bool ChatRoom::EventLoop::hasChannel(Channel * ch)
{
	return epoll_->hasChannel(ch);
}

void ChatRoom::EventLoop::pendingFunctor(Functor & fun)
{
	pendingFunctorGuard(fun);
}

void ChatRoom::EventLoop::pendingFunctorGuard(Functor& fun)
{
	MutexLockGuard guard(mutex_);
	pendingFunctor_.push_back(fun);
}
