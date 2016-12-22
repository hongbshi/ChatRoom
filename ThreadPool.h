#ifndef  ChatRoom_ThreadPool_H
#define ChatRoom_ThreadPool_H
#include<map>
namespace ChatRoom
{
	class EventLoop;
	class ThreadPool
	{
	public:
		typedef std::is_function<void()> ThreadFunctor;
		ThreadPool(int threadNum=0);
		~ThreadPool();
		void start();
		EventLoop* getNext();
	private:
		int threadNum;
		ThreadFunctor startFun_;
		std::map < EventLoop*, std::string> threads_;
	};
}
#endif // ! ChatRoom_ThreadPool_H
