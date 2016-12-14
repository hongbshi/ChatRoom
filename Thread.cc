#include"Thread.h"

#include<cstddef>
#include<sys/syscall.h>
#include<unistd.h>
#include<pthread.h>
#include<stdio.h>

namespace ChatRoom
{
	__thread pid_t _tId=0;
	void afterfork()
	{
		_tId = static_cast<pid_t>(::syscall(SYS_gettid));
	}
	class initial
	{
		public:
		initial()
		{
			_tId = static_cast<pid_t>(::syscall(SYS_gettid));
		        pthread_atfork(NULL, NULL, &afterfork);
		}

	};
	initial ini;
	pid_t getCurrentThreadTid()
	{
		if(_tId==0)
			_tId = static_cast<pid_t>(::syscall(SYS_gettid));
		return _tId;
	}
}

