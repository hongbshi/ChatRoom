#ifndef  ChatRoom_MutexLock_H
#define  ChatRoom_MutexLock_H

#include <pthread.h>
namespace ChatRoom
{
	class MutexLock
	{
	public:
		MutexLock();
		void Lock();
		void unLock();
		pthread_mutex_t* getPthreadMutex();
		~MutexLock();
	private:
		pthread_mutex_t mutex_;
	};
}
#endif // ! ChatRoom_MUTEXLOCK_H
