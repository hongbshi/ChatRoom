#ifndef  ChatRoom_Thread_H
#define  ChatRoom_Thread_H

#include<cstddef>
#include<sys/syscall.h>
#include<unistd.h>
#include<pthread.h>
#include<string>
#include<functional>
#include "MutexLock.h"
namespace ChatRoom
{
	pid_t getCurrentThreadTid();

	class Thread
	{
	public:
		typedef std::function<void()> ThreadFunctor;
		Thread(ThreadFunctor fun);
		~Thread();
		int start();
		void join();
		std::string getName();
		pid_t getTid();
		pthread_t getThreadId();
		bool isStart();
		bool isJoin();
	private:
		std::string defaultName();
		ThreadFunctor startFun_;
		std::string name_;
		std::shared_ptr<pid_t> tid_;
		pthread_t pthreadId_;
		bool start_;
		bool join_;
		MutexLock mutex_;
		static int incrementNumber;
	};
}
#endif // ! ChatRoom_Thread_H
