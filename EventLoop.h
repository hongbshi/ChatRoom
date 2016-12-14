#ifndef  ChatRoom_EventLoop_H
#define ChatRoom_EventLoop_H

#include<vector>
#include<memory>
#include<unistd>
#include "MutexLock.h"
namespace ChatRoom
{
	class Channel;
	class Epoll;
	class EventLoop
	{
	public:
		typedef void(*Functor)();
		EventLoop();
		void loop();
		void wakeup();
		void removeChannle(Channel* ch);
		void updateChannle(Channel* ch);
		bool hasChannel(Channel* ch);
		void setEpollTimeoutMs(int timeout)
		{
			kEpollTimeoutMs_ = timeout;
		}
		void quit()
		{
			quit_ = true;
		}
		void pendingFunctor(Functor& fun);
	private:
		void pendingFunctorGuard(Functor& fun);
		int kEpollTimeoutMs_;
		pid_t tid_;
		//status
		bool looping_;
		bool quit_;
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

