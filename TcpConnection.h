#ifndef  ChatRoom_TcpConnection_H
#define ChatRoom_TcpConnection_H

#include<netinet/in.h>
#include<memory>
#include "Channel.h"
#include "Buffer.h"
#include "EventLoop.h"
namespace ChatRoom
{
	//have connected
	class TcpConnection
	{
	public:
		typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
		typedef std::function<void(TcpConnectionPtr)> ConnectedCallback;
		typedef std::function<void(TcpConnectionPtr)> CloseCallback;
		//use send to write data
		typedef std::function<void(TcpConnectionPtr)> WriteCallback;
		//use getOutputBuffer first
		typedef std::function<void(TcpConnectionPtr)> MessageCallback;

		TcpConnection(EventLoop* loop,int sockfd,
		struct sockaddr_in localAddress,
		struct sockaddr_in peerAddress);
		~TcpConnection();

		//when connect was completed,it was been called.
		void setConnectedCallback(const ConnectedCallback& cb)
		{
			connectedCallback_ = cb;
		}

		void setCloseCallback(const CloseCallback& cb)
		{
			closeCallback_ = cb;
		}

		void setWriteCallback(const WriteCallback& cb)
		{
			writeCallback_ = cb;
		}

		void setMessageCallback(const MessageCallback& cb)
		{
			messageCallback_ = cb;
		}

		void connectEstablished();
		void connectDestroyed();

		void send(std::string& s);
		//void send(std::string&& s);
		void send(Buffer& buff);
		//void send(Buffer&& buff);

		void close();
		void shutdownWrite();
		void startRead();
		void stopRead();
		//void forceClose();

		Buffer* getInputBuffer() { return &inputBuffer_; }
		Buffer* getOutputBuffer() { return &outputBuffer_; }
		EventLoop* getLoop() { return loop_; }

		const struct sockaddr_in& getLocalAddress() { return localAddress_; }
		const struct sockaddr_in& getPeerAddress() { return peerAddress_; }

		bool connected() { return sockState_ == kConnected; }
		bool disConnected() { return sockState_ == kDisconnected; }
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
		enum sockStates
		{
			kConnecting, kConnected, kDisconnecting, kDisconnected
		};
		EventLoop* loop_;
		int sockfd_;
		struct sockaddr_in localAddress_;
		struct sockaddr_in peerAddress_;

		sockStates sockState_;
		std::shared_ptr<Channel> channel_;
		ConnectedCallback connectedCallback_;
		CloseCallback closeCallback_;
		WriteCallback writeCallback_;
		MessageCallback messageCallback_;

		Buffer inputBuffer_;
		Buffer outputBuffer_;
	};
}
#endif // ! ChatRoom_TcpConnection_H
