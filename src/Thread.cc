#include "Thread.h"
#include "EventLoop.h"
#include "MutexLockGuard.h"
#include "Condition.h"
#include <cstddef>
#include <sys/syscall.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <memory>
using namespace ChatRoom;

__thread pid_t _tId = 0;

void afterfork()
{
	_tId = static_cast<pid_t>(syscall(SYS_gettid));
}

class ThreadTidInitial
{
	public:
	ThreadTidInitial()
	{
		_tId = static_cast<pid_t>(syscall(SYS_gettid));
	    pthread_atfork(NULL, NULL, &afterfork);
	}
};
ThreadTidInitial threadTidInitial;

pid_t ChatRoom::getCurrentThreadTid()
{
	if(_tId == 0) _tId = static_cast<pid_t>(syscall(SYS_gettid));
	return _tId;
}

////////////////////////////////
ChatRoom::Thread::Thread(ThreadInitial initialCallback):
	mutex_(), cond_(mutex_)
{
	initialFun_ = initialCallback;
	tid_ = 0;
	pthreadId_ = 0;
	loop_ = nullptr;
}

ChatRoom::Thread::~Thread()
{
	loop_->quit();
}

struct ThreadData
{
	typedef ChatRoom::Thread::Functor ThreadFunctor;
	ThreadFunctor fun_;
	ThreadData(ThreadFunctor fun)
	{
		fun_ = fun;
	}
};

EventLoop* ChatRoom::Thread::startloop()
{
	ThreadData *data = new ThreadData(std::bind(&Thread::run,this));
	int result = pthread_create(&pthreadId_, NULL, &internalThreadStart, data);
	if (result < 0)
	{
		//deal error
		delete data;
		return nullptr;
	}
	else
	{
		MutexLockGuard guard(mutex_);
		while (loop_ == nullptr) cond_.wait();
		return loop_;
	}
}

void ChatRoom::Thread::run()
{
	tid_ = getCurrentThreadTid();
	EventLoop loop;
	{
		MutexLockGuard guard(mutex_);
		loop_ = &loop;
		cond_.notify();
	}
	if (initialFun_) initialFun_(loop_);
	loop.loop();
}

void* ChatRoom::internalThreadStart(void * data)
{
	ThreadData *threadData = (ThreadData*)(data);
	//run the fun
	threadData->fun_();
	delete threadData;
}

