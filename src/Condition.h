#ifndef  ChatRoom_Condition_H
#define  ChatRoom_Condition_H

#include <pthread.h>
#include "MutexLock.h"
namespace ChatRoom
{
	class Condition
	{
	public:
		Condition(MutexLock& lock);
		~Condition();
		void wait();
		void notify();
		void notifyAll();
	private:
		MutexLock& lock_;
		pthread_cond_t cond_;
	};
}
#endif // ! ChatRoom_Condition_H
