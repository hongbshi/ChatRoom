#include "MutexLock.h"

using namespace ChatRoom;
MutexLock::MutexLock()
{
	pthread_mutex_init(&mutex_,nullptr);
}

void MutexLock::Lock()
{
	pthread_mutex_lock(&mutex_);
}

void MutexLock::unLock()
{
	pthread_mutex_unlock(&mutex_);
}

pthread_mutex_t * ChatRoom::MutexLock::getPthreadMutex()
{
	return &mutex_;
}

MutexLock::~MutexLock()
{
	pthread_mutex_destroy(&mutex_);
}