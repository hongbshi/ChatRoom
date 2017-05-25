#ifndef  ChatRoom_MutexLockGuard_H
#define  ChatRoom_MutexLockGuard_H

namespace ChatRoom
{
	class MutexLock;
	class MutexLockGuard
	{
	public:
		MutexLockGuard(MutexLock& lock);
		~MutexLockGuard();
	private:
		MutexLock& lock_;
	};
}
#endif // ! ChatRoom_MUTEXLOCK_H
