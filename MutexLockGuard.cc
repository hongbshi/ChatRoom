#include "MutexLockGuard.h"
#include "MutexLock.h"

using namespace ChatRoom;

MutexLockGuard::MutexLockGuard(MutexLock& lock):lock_(lock)
{
	lock_.Lock();
}

MutexLockGuard::~MutexLockGuard()
{
	lock_.unLock();
}