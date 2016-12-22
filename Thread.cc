#include"Thread.h"
#include "MutexLockGuard.h"
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
	int  ChatRoom::Thread::incrementNumber=1;
	ChatRoom::Thread::Thread(ThreadFunctor fun)
	{
		startFun_ = fun;
		name_ = defaultName();
		tid_ = std::make_shared<pid_t>(0);
		pthreadId_=0;
		start_ = false;
		join_ = false;
	}

	struct ThreadData
	{
		typedef ChatRoom::Thread::ThreadFunctor ThreadFunctor;
		ThreadFunctor fun_;
		std::string name_;
		std::weak_ptr<pid_t> tid_;
		ThreadData(ThreadFunctor fun, const std::string& name, std::shared_ptr<pid_t> tid)
		{
			fun_ = fun;
			name_ = name;
			tid_ = tid;
		}
	};

	int Thread::start()
	{
		ThreadData* data=new ThreadData(startFun_, name_, tid_);
		int result = pthread_create(&pthreadId_, NULL, &internalThreadStart, data);
		if (result < 0)
		{
			//deal error
			delete data;
		}
		return result;
	}

	void* internalThreadStart(void* data)
	{
		ThreadData* thread = (ThreadData*)(data);
		//lock weak_ptr, write tid for Thread
		//run the fun
	}

	std::string Thread::defaultName()
	{
		int num;
		{
			MutexLockGuard guard(mutex_);
			num=incrementNumber++;
		}
		return std::string();
	}
}

