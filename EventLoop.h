#ifndef  ChatRoom_EventLoop_H
#define ChatRoom_EventLoop_H

#include<vector>
#include<memory>
namespace ChatRoom
{
	class Channel;
	class Epoll;
	class EventLoop
	{
	public:
		typedef void(*Functor)();
		EventLoop();
		void loop();
		void removeChannle(Channel* ch);
		void updateChannle(Channel* ch);
		bool hasChannel(Channel* ch);
	private:
		bool quit_;

		int wakeupfd_;
		Channel wakeupChannel_;

		shared_ptr<Epoll> epoll_;
		std::vector<Channel*> activeChannel_;
		std::vector<Functor> pendingFunctor_;
	};
}



#endif // ! ChatRoom_EventLoop_H

