#include <iostream>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <arpa/inet.h>

#include "Buffer.h"
#include "Socket.h"
#include "Thread.h"
#include "HttpServer.h"
#include "EventLoop.h"
#include "Connector.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "TcpConnection.h"

using namespace std;
using namespace ChatRoom;
using namespace std::placeholders;

typedef std::function<void(int)> ConnCb;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void(TcpConnectionPtr)> ConnectedCallback;
typedef std::function<void(TcpConnectionPtr)> CloseCallback;
typedef std::function<void(TcpConnectionPtr)> WriteCallback;
typedef std::function<void(TcpConnectionPtr, Buffer*)> MessageCallback;

EventLoop *mainLoop = nullptr;

void newConnCb(TcpConnectionPtr ptr){
	printf("File: HttpClientTest, newConnCb function.\n");
	HttpRequest req;
	req.setMethod(HttpRequest::kGet);
	req.setUrl(string("/"));
	req.setVersion(HttpRequest::kHttp11);
	Buffer buff;
	req.appendToBuff(buff);
	ptr->send(buff);
}

void closeCb(TcpConnectionPtr ptr){
	printf("File: HttpClientTest, newConnCb function.\n");
}

void writeCb(TcpConnectionPtr ptr){
	//
	printf("File: HttpClientTest, writeCb function.\n");
}

void messageCb(TcpConnectionPtr ptr, Buffer * buff){
	printf("File: HttpClientTest, messageCb function.\n");
	printf("%s\n", buff->getString().c_str());
}

void connCb(int sockfd){
	printf("File: HttpClientTest, connector connCb function.\n");
	std::string name{"client TcpConnection"}; 
	struct sockaddr localAddr = getLocalAddr(sockfd);
	struct sockaddr peerAddr = getPeerAddr(sockfd);
	TcpConnectionPtr ptr = make_shared<TcpConnection> (mainLoop,sockfd,
		sockaddr_in_cast(&localAddr),sockaddr_in_cast(&peerAddr),name);
	ptr->setConnectedCallback(std::bind(&newConnCb, _1));
	ptr->setCloseCallback(std::bind(&closeCb, _1));
	ptr->setWriteCallback(std::bind(&writeCb, _1));
	ptr->setMessageCallback(std::bind(&messageCb, _1, _2));
}

int main(int argc,char *argv[]){
	for(int i = 0;i < argc; ++i) cout << argv[i] <<endl;
	if(argc != 3) {
		printf("%s\n", "<Ip> <Port>"); 
		return -1;
	}
	//Initial server address
	struct sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(serverAddr));
	int port = atoi(argv[2]);
  	serverAddr.sin_family = AF_INET;
  	serverAddr.sin_port = htons(port);
 	int result = inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);
 	if(result < 1) printf("Main function error! ip or port wrong.\n");
 	//Initial variable
 	EventLoop loop;
 	mainLoop = &loop;
 	Connector connector(&loop,serverAddr);
 	connector.setConnectionCallback(connCb);
 	connector.start();
 	loop.loop();
 	return 0;
}

