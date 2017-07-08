#ifndef  ChatRoom_Connector_H
#define  ChatRoom_Connector_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <utility>
#include <mutex>
#include <memory>

namespace ChatRoom
{
	class Channel;
	class EventLoop;
	class Connector
	{
	public:
		typedef std::function<void(int)> ConnCb;

		enum States {
			kDisConnected, 
			kDisConnecting, 
			kConnecting, 
			kConnected
		};

		Connector(EventLoop *loop, const struct sockaddr_in & server);
		~Connector();

		void setConnectionCallback(ConnCb & cb){ connCb_ = cb;}
		void setConnectionCallback(ConnCb && cb){ connCb_ = std::move(cb);}

		void start();
		void stop();

		bool isConn(){ 
			std::lock_guard<std::mutex> guard(mu_);
			return state_ == kConnected;
		}

	private:
		void startInloop();
		void stopInloop();

		void setState(States s){
			std::lock_guard<std::mutex> guard(mu_);
			state_ = s;
		}

		int resetChannel();
		void handleWrite();
		
		EventLoop *loop_;
		struct sockaddr_in serverAddr_;
		ConnCb connCb_;
		std::mutex mu_;
		States state_;
		Channel *ch_;  //
	};
}
#endif // ! ChatRoom_Connector_H
