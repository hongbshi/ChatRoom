#ifndef  ChatRoom_Channel_H
#define ChatRoom_Channel_H

#include<sys/epoll.h>
namespace ChatRoom
{
	const int kNew = -1;
	const int kAdded = 1;
	const int kDeleted = 2;
	class Channel
	{
	public:
		typedef void(*EventCallback)();
		Channel(int fd);
		void setReadCallback(EventCallback& rcb)
		{
			readCallback_ = rcb;
		}
		void setWriteCallback(EventCallback& wcb)
		{
			writeCallback_ = wcb;
		}
		void setCloseCallback(EventCallback& ccb)
		{
			closeCallback_ = ccb;
		}
		void setErrorCallback(EventCallback& ecb)
		{
			errorCallback_ = ecb;
		}

		void enableRead()
		{
			Event_ |=  kReadEvent;
		}
		void disableRead()
		{
			Event_ &= ~kReadEvent;
		}
		void enableWrite()
		{
			Event_ |= kWriteEvent;
		}
		void disableWrite()
		{
			Event_ &= ~kWriteEvent;
		}
		void disableAll()
		{
			Event_ &= kNoneEvent;
		}
		void enableAll()
		{
			Event_ &= kReadEvent;
			Event_ &= kWriteEvent;
		}

		bool isRead()
		{
			return Event_ & kReadEvent;
		}
		bool isWrite()
		{
			return Event_ & kWriteEvent;
		}
		bool isNoneEvent()
		{
			return Event_ & (~kNoneEvent);
		}
		int getfd()
		{
			return fd_;
		}
		int getEvent()
		{
			return Event_;
		}
		int getStatus()
		{
			return status_;
		}
		void setStatus(int status)
		{
			status_ = status;
		}
		void setReEvent(int revent)
		{
			reEvent_ = revent;
		}
		void handleEvent();
	private:
		static int const kReadEvent;
		static int const kWriteEvent;
		static int const kNoneEvent;
		EventCallback readCallback_;
		EventCallback writeCallback_;
		EventCallback closeCallback_;
		EventCallback errorCallback_;
		int Event_;   //epoll listen event
		int reEvent_;    //receive event from epoll 
		int fd_;        //listen fd
		int status_; //for epoll to use
		
	};
}
#endif // ! ChatRoom_Channel_H

