#include "Condition.h"
#include "MutexLock.h"
ChatRoom::Condition::Condition(MutexLock & lock)
{
	pthread_cond_init(&cond_, nullptr);
	lock_ = lock;
}

ChatRoom::Condition::~Condition()
{
	pthread_cond_destroy(&cond);
}

void ChatRoom::Condition::wait()
{
	pthread_cond_wait(&cond_,lock_.getPthreadMutex());
}

void ChatRoom::Condition::notify()
{
	pthread_cond_signal()
}

void ChatRoom::Condition::notifyAll()
{
	pthread_cond_broadcast(&cond_);
}
