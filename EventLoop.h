#ifndef  ChatRoom_EventLoop_H
#define ChatRoom_EventLoop_H

#include<vector>
#include<memory>
#include<unistd.h>
#include "MutexLock.h"
#include "MutexLockGuard.h"
namespace ChatRoom
{
	class Channel;
	class Epoll;
	class EventLoop
	{
	public:
		typedef std::function<void()> Functor;
		EventLoop();
		void loop();
		void quit()
		{
			quit_ = true;
			if (!isInLoopthread())
				wakeup();
		}
		void setEpollTimeoutMs(int timeout)
		{
			MutexLockGuard guard(mutex_);
			kEpollTimeoutMs_ = timeout;
		}
		void runInLoop(Functor& fun);
		void queueInLoop(Functor& fun);
		//Thread safe
		bool isInLoopthread();
	private:
		//internal use
		void wakeup();
		void removeChannle(Channel* ch);
		void updateChannle(Channel* ch);
		bool hasChannel(Channel* ch);
		void doPendingFunctors();
		void wakeupChannelReadCallback();
		//internal variable
		int kEpollTimeoutMs_;
		pid_t tid_;
		//status
		bool looping_;
		bool quit_;
		bool callingPendingFunctors_;
		//wakeup the loop
		//eventfd
		int wakeupfd_;  
		std::shared_ptr<Channel> wakeupChannel_;
		//run the epoll
		std::shared_ptr<Epoll> epoll_;
		std::vector<Channel*> activeChannel_;
		//Guard pendingFunctor
		MutexLock mutex_;
		std::vector<Functor> pendingFunctor_;
	};
}
#endif // ! ChatRoom_EventLoop_H

