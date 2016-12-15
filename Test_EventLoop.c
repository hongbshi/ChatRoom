#include<iostream>
#include<stdio.h>
#include<functional>
#include<pthread.h>
#include "EventLoop.h"
#include "MutexLock.h"
#include "MutexLockGuard.h"
#include "Thread.h"

using namespace ChatRoom;
MutexLock mutex;
EventLoop* childLoop=NULL;
void* childThread(void*);
void* ChildThread(void*)
{
	EventLoop childEventLoop;
	{
		MutexLockGuard guard(mutex);
		childLoop=&childEventLoop;
	}
	std::cout<<"Print Child Thread Tid"<<std::endl;
        std::cout<<getCurrentThreadTid()<<std::endl;
	childEventLoop.loop();
	std::cout<<"The Child Thread quit"<<std::endl;
        return NULL;
}
void insertFun()
{
	std::cout<<"This is insertFun"<<std::endl;
	std::cout<<getCurrentThreadTid()<<std::endl;
}
int main()
{
	pthread_t child;
	pthread_create(&child,NULL,ChildThread,NULL);
	bool flag=false;
	while(!flag)
	{
		MutexLockGuard guard(mutex);
		if(childLoop!=NULL)
			flag=true;
	}
	std::function<void()> insert(insertFun);
	//Child Thread not in poll,parent thread excute quit,what happend?
	childLoop->runInLoop(insert);
	childLoop->quit();
	pthread_join(child,NULL);
	std::cout<<"The main Thread quit"<<std::endl;

}
