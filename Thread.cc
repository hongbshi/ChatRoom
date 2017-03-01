#include "Thread.h"
#include "EventLoop.h"
#include "MutexLockGuard.h"
#include "Condition.h"
#include<cstddef>
#include<sys/syscall.h>
#include<unistd.h>
#include<pthread.h>
#include<stdio.h>
#include<memory>
namespace ChatRoom
{
	__thread pid_t _tId=0;
	void afterfork()
	{
		_tId = static_cast<pid_t>(::syscall(SYS_gettid));
	}
	class initial
	{
		public:
		initial()
		{
			_tId = static_cast<pid_t>(::syscall(SYS_gettid));
		    pthread_atfork(NULL, NULL, &afterfork);
		}
	};
	initial ini;
	pid_t getCurrentThreadTid()
	{
		if(_tId==0)
			_tId = static_cast<pid_t>(::syscall(SYS_gettid));
		return _tId;
	}

	////////////////////////////////
	ChatRoom::Thread::Thread(ThreadInitial initialCallback):
		mutex_(),cond_(mutex_)
	{
		initialFun_ = initialCallback;
		tid_ = nullptr;
		pthreadId_=0;
		loop_ = nullptr;
	}

	Thread::~Thread()
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

	EventLoop* Thread::startloop()
	{
		ThreadData* data=new ThreadData(std::bind(&Thread::run,this));
		int result = pthread_create(&pthreadId_, NULL, &internalThreadStart, data);
		if (result < 0)
		{
			//deal error
			delete data;
			return nullptr;
		}
		else
		{
			//循环等待子线程
			MutexLockGuard guard(mutex_);
			while (loop_ == nullptr)
				cond_.wait();
			return loop_;
		}
	}

	void Thread::run()
	{
		tid_ = getCurrentThreadTid();
		EventLoop loop;
		//通知父线程
		{
			MutexLockGuard guard(mutex_);
			loop_ = &loop;
			cond_.notify();
		}
		//执行initialcallback
		if (initialFun_)
			initialFun_(loop_);
		//执行事件循环
		loop.loop();
	}

	void* internalThreadStart(void* data)
	{
		ThreadData* threadData = (ThreadData*)(data);
		//run the fun
		threadData->fun_();
		delete threadData;
	}
}

