#ifndef  ChatRoom_Thread_H
#define  ChatRoom_Thread_H

#include<cstddef>
#include<sys/syscall.h>
#include<unistd.h>
#include<pthread.h>
namespace ChatRoom
{
	pid_t getCurrentThreadTid();
}
#endif // ! ChatRoom_Thread_H
