#ifndef  ChatRoom_Acceptor_H
#define ChatRoom_Acceptor_H

#include<memory>
#include<functional>
#include "Channel.h"
namespace ChatRoom
{
	class Channel;
	class EventLoop;
	class Acceptor
	{
	public:
		typedef std::function<void(int sockfd,
			const struct sockaddr& clientAddr)> NewConnectCallback;

		Acceptor(EventLoop* loop,
			const struct sockaddr* listenAddr,
			bool reusePort);
		void setNewConnectCallback(NewConnectCallback&& cb);
		bool isListening();
		void listen();
		~Acceptor();
	private:
		void handleRead();
		int sockfd_;
		//occupied fd for use
		int idlefd_;
		EventLoop* loop_;
		Channel channel_;
		NewConnectCallback newConnectCallback_;
		bool listening_;
	};
}
#endif // ! ChatRoom_Acceptor_H
