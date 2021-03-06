#include "EventLoop.h"
#include "Epoll.h"
#include "MutexLockGuard.h"
#include "Channel.h"
#include "Thread.h"
#include <sys/eventfd.h>
#include <memory>
#include <functional>
#include <unistd.h>
using namespace ChatRoom;

__thread bool t_loopInThisThread = false;

EventLoop::EventLoop()
{
	if (t_loopInThisThread) abort();  //To Do
	kEpollTimeoutMs_ = 1000;
	tid_ = getCurrentThreadTid();
	t_loopInThisThread = true;
	looping_ = false;
	quit_ = false;
	callingPendingFunctors_ = false;
	wakeupfd_ = eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
	wakeupChannel_ = std::make_shared<Channel>(wakeupfd_);
	wakeupChannel_->enableRead();
	epoll_ = std::make_shared<Epoll>();
	epoll_->updateChannel(&*wakeupChannel_);
	wakeupChannel_->setReadCallback(std::bind(&EventLoop::wakeupChannelReadCallback,this));
}

void ChatRoom::EventLoop::loop()
{
	if (looping_) abort();  //TO DO
	looping_ = true;
	while (!quit_)
	{
		activeChannel_.clear();
		epoll_->poll(kEpollTimeoutMs_, activeChannel_);
		int len = activeChannel_.size();
		printf("File: EventLoop.cc, loop function, %d events happend in thread %d.\n", len, tid_);
		for (int i = 0; i < len; i++)
		{
			activeChannel_[i]->handleEvent();
		}
		doPendingFunctors();
	}
}

void ChatRoom::EventLoop::wakeup()
{
	int i = 1;
	write(wakeupfd_, &i, sizeof i);
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

bool ChatRoom::EventLoop::isInLoopthread()
{
	return tid_ == ChatRoom::getCurrentThreadTid();
}

void ChatRoom::EventLoop::runInLoop(const Functor & fun)
{
	if (isInLoopthread()) fun();
	else queueInLoop(fun);
}

void ChatRoom::EventLoop::queueInLoop(const Functor& fun)
{
	{
		MutexLockGuard guard(mutex_);
		pendingFunctor_.push_back(fun);
	}
	if (!isInLoopthread() || !callingPendingFunctors_) wakeup();
}

void ChatRoom::EventLoop::doPendingFunctors()
{
	printf("File: EventLoop.cc, doPendingFunctors funtion start.\n");
	callingPendingFunctors_ = true;
	std::vector<Functor>  tmp;
	{
		MutexLockGuard guard(mutex_);
		tmp.swap(pendingFunctor_);
	}
	for (int i = 0; i < tmp.size(); i++) tmp[i]();
	callingPendingFunctors_ = false;
}

void ChatRoom::EventLoop::wakeupChannelReadCallback()
{
	int i = 1;
	read(wakeupfd_, &i, sizeof(i));
}
