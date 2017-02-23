#ifndef  ChatRoom_Thread_H
#define  ChatRoom_Thread_H

#include<cstddef>
#include<sys/syscall.h>
#include<unistd.h>
#include<pthread.h>
#include<string>
#include<functional>
#include "MutexLock.h"
#include "Condition.h"
namespace ChatRoom
{
	pid_t getCurrentThreadTid();
	class EventLoop;
	class Thread
	{
	public:
		typedef std::function<void()> ThreadFunctor;
		Thread(ThreadFunctor initialCallback=0);
		~Thread();
		EventLoop* startloop();
	private:
		//子线程执行
		void run();
		ThreadFunctor initialFun_;
		pid_t tid_;
		pthread_t pthreadId_;
		EventLoop* loop_;
		MutexLock mutex_;
		Condition cond_;
	};
}
#endif // ! ChatRoom_Thread_H
