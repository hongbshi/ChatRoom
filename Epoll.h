#ifndef  ChatRoom_Epoll_H
#define  ChatRoom_Epoll_H

#include <sys/epoll.h> 
#include <map>
#include <vector>

#define Epoll_Listen_Size 1024
namespace ChatRoom
{
	class Channel;
	class Epoll
	{
	public:
		Epoll();
		//Add to activeChannel
		void poll(int timeout,std::vector<Channel*>& activeChannel);
		//If ch.status is kNew or kDeleted, add ch to listen. otherwise update the channel status.
		void updateChannel(Channel* ch); 
		//ch must be stay in activeChannel_ ,otherwise assert failed
		void removeChannel(Channel* ch); 
		//activeChannel_ has ch
		bool hasChannel(Channel* ch);
	private:
		void update(int operation, Channel* ch);
		void fillActiveChannel(std::vector<Channel*>& activeChannel);
		typedef std::vector<epoll_event> EventList;
		int epollfd_;
		EventList event_;
		int initialSize;
		std::map<int, Channel*> activeChannel_;   //fd,channel*
	};
}

#endif // ! CharRoom_Epoll_H

