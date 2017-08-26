#ifndef  ChatRoom_TcpConnection_H
#define  ChatRoom_TcpConnection_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <memory>

#include "Channel.h"
#include "Buffer.h"
#include "EventLoop.h"
namespace ChatRoom
{
	//have connected
	class TcpConnection: public std::enable_shared_from_this<TcpConnection>
	{
	private:
		enum sockStates
		{
			kConnecting, kConnected, kDisconnecting, kDisconnected
		};

	public:
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		typedef std::function<void(TcpConnectionPtr)> ConnectedCallback;
		typedef std::function<void(TcpConnectionPtr)> CloseCallback;
		//use send to write data
		typedef std::function<void(TcpConnectionPtr)> WriteCallback;
		//use getOutputBuffer first
		typedef std::function<void(TcpConnectionPtr, Buffer*)> MessageCallback;

		//
		typedef std::function<void(TcpConnectionPtr)> ReadZeroCallback;

		TcpConnection(EventLoop* loop,
		int sockfd,
		const struct sockaddr_in * localAddress,
		const struct sockaddr_in * peerAddress, 
		const std::string & name);
		~TcpConnection();

		//when connect was completed,it was been called.
		void setConnectedCallback(const ConnectedCallback & cb){
			connectedCallback_ = cb;
		}

		void setCloseCallback(const CloseCallback & cb){
			closeCallback_ = cb;
		}

		void setWriteCallback(const WriteCallback & cb){
			writeCallback_ = cb;
		}

		void setMessageCallback(const MessageCallback & cb){
			messageCallback_ = cb;
		}

		void setReadZeroCallback(const ReadZeroCallback & cb){
			readZeroCallback_ = cb;
		}

		void setStates(sockStates states) { sockState_ = states; }
		static int getNumber() { return ++number; }
		const std::string & getName() { return name_; }
		void connectEstablished();
		void connectDestroyed();

		void send(std::string & s);
		void send(std::string && s);
		void send(Buffer & buff);
		void send(const Buffer & buff);

		void close();
		void shutdownWrite();

		//start the read and stop the read
		void startRead();
		void stopRead();

		void startWrite();
		void stopWrite();

		Buffer* getInputBuffer() { return &inputBuffer_; }
		Buffer* getOutputBuffer() { return &outputBuffer_; }

		EventLoop* getLoop() { return loop_; }

		const struct sockaddr_in& getLocalAddress() { return localAddress_; }
		const struct sockaddr_in& getPeerAddress() { return peerAddress_; }

		bool connected() { return sockState_ == kConnected; }
		bool disConnected() { return sockState_ == kDisconnected; }

		void setContext(std::shared_ptr<void> ptr) {
			context_ = ptr;
		}

		std::shared_ptr<void> getContext() {
			return context_;
		}
		
		void setWeakContext(std::shared_ptr<void> ptr){
			weakContext_ = ptr;
		}

		std::weak_ptr<void> getWeakContext(){
			return weakContext_;
		}

	private:
		void handleRead();
		void handleWrite();
		void handleError();
		void handleClose();
		void sendInLoop(std::string& s);
		void shutdownWriteInLoop();
		void closeInLoop();
		void startReadInLoop();
		void stopReadInLoop();
		void startWriteInLoop();
		void stopWriteInLoop();

		static void defaultMessageCb(TcpConnectionPtr, Buffer*);

		EventLoop *loop_;

		int sockfd_;
		struct sockaddr_in localAddress_;
		struct sockaddr_in peerAddress_;
		std::string name_;
		sockStates sockState_;
		std::shared_ptr<Channel> channel_;

		ConnectedCallback connectedCallback_;
		CloseCallback closeCallback_;
		WriteCallback writeCallback_;
		MessageCallback messageCallback_;
		ReadZeroCallback readZeroCallback_;

		Buffer inputBuffer_;
		Buffer outputBuffer_;

		static int number;

		std::shared_ptr<void> context_;
		std::weak_ptr<void> weakContext_;
	};
}
#endif // ! ChatRoom_TcpConnection_H
