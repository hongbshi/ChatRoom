#ifndef  ChatRoom_Connector_H
#define  ChatRoom_Connector_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <utility>
#include <mutex>

namespace ChatRoom
{
	class Channel;
	class EventLoop;
	class Connector
	{
	public:
		typedef std::function<void(int)> ConnCb;

		Connector(EventLoop *loop, const struct sockaddr_in & server);
		~Connector();

		void setConnectionCallback(ConnCb & cb){ connCb_ = cb;}
		void setConnectionCallback(ConnCb && cb){ connCb_ = std::move(cb);}

		void start();
		void stop();

		bool isConn(){ 
			std::lock_guard guard(mu_);
			return state_ == kConnected;
		}

	private:
		void startInloop();
		void stopInloop();

		void setState(States s){
			std::lock_guard guard(mu_);
			state_ = s;
		}

		void resetChannel();
		void handleWrite();

		enum States{kDisconnected, kDisConnecting, kConnecting, kConnected};
		
		EventLoop *loop_;
		struct sockaddr_in serverAddr_;
		ConnCb connCb_;
		std::mutex mu_;
		States state_;
		shared_ptr<Channel> ch_;  //
	};
}
#endif // ! ChatRoom_Connector_H
