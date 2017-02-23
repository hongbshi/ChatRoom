#ifndef  ChatRoom_ThreadPool_H
#define ChatRoom_ThreadPool_H
#include<vector>
#include<functional>
#include<memory>
namespace ChatRoom
{
	class Thread;
	class EventLoop;
	class ThreadPool
	{
	public:
		typedef std::function<void()> ThreadFunctor;
		ThreadPool(ThreadFunctor initialCallback=0,const unsigned int threadNum=1);
		~ThreadPool();
		void start();
		EventLoop* getNext();
	private:
		const unsigned int threadNum_;
		unsigned int nextNum_;
		ThreadFunctor startFun_;
		std::vector<std::shared_ptr<Thread>> threads_;
		std::vector <EventLoop*> loops_;
	};
}
#endif // ! ChatRoom_ThreadPool_H
