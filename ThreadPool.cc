#include "ThreadPool.h"
#include "Thread.h"
#include "EventLoop.h"

ChatRoom::ThreadPool::ThreadPool(EventLoop* loop,
	ThreadFunctor initialCallback, 
	const unsigned int threadNum):
	loop_(loop),
	startFun_(initialCallback),
	threadNum_(threadNum),
	loops_(threadNum,nullptr),
	nextNum_(0)
{
	for (int i = 0; i < threadNum_; ++i)
		threads_.push_back(std::make_shared<Thread>(startFun_));
}

ChatRoom::ThreadPool::~ThreadPool()
{
	//
	for (int i = 0; i < threadNum_; ++i)
		loops_[i]->quit();
}

void ChatRoom::ThreadPool::start()
{
	//Deal error
	for (int i = 0; i < threadNum_; ++i)
		loops_[i] = threads_[i]->startloop();
}

using namespace ChatRoom;
EventLoop* ChatRoom::ThreadPool::getNext()
{
	if(!threadNum_)
		return loop_;
	else
	{
		EventLoop* tmp=loops_[nextNum_];
		nextNum_ = (nextNum_ + 1) % threadNum_;
		return tmp;
	}
}
