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
#include "TcpClient.h"
#include "HttpServer.h"
#include "EventLoop.h"
#include "Connector.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "TcpConnection.h"

using namespace std;
using namespace ChatRoom;
using namespace std::placeholders;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void(TcpConnectionPtr)> ConnectedCallback;
typedef std::function<void(TcpConnectionPtr)> CloseCallback;
typedef std::function<void(TcpConnectionPtr)> WriteCallback;
typedef std::function<void(TcpConnectionPtr, Buffer*)> MessageCallback;

void newConnCb(TcpConnectionPtr ptr){
	printf("File: HttpClientTest, newConnCb function.\n");
	HttpRequest req;
	req.setMethod(HttpRequest::kGet);
	req.setUrl(string("/HttpClientTest"));
	req.setVersion(HttpRequest::kHttp11);
	Buffer buff;
	req.appendToBuff(buff);
	ptr->send(buff);
	printf("File: HttpClientTest, newConnCb function end.\n");
}

void closeCb(TcpConnectionPtr ptr){
	printf("File: HttpClientTest, closeCb function.\n");
}

void writeCb(TcpConnectionPtr ptr){
	//
	printf("File: HttpClientTest, writeCb function.\n");
}

void messageCb(TcpConnectionPtr ptr, Buffer * buff){
	printf("File: HttpClientTest, messageCb function.\n");
	printf("%s\n", buff->getString().c_str());
}

int main(int argc,char *argv[]){
	//for(int i = 0;i < argc; ++i) cout << argv[i] <<endl;
	if(argc != 3) {
		printf("%s\n", "<Server Ip> <Server Port>"); 
		return -1;
	}
	//Initial server address
	struct sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(serverAddr));
	int port = atoi(argv[2]);
  	serverAddr.sin_family = AF_INET;
  	serverAddr.sin_port = htons(port);
 	int result = inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);
 	if(result < 1) printf("Server Ip or Server Port is wrong.\n");
 	//Initial variable
 	EventLoop loop;
	TcpClient client(&loop,serverAddr);
	client.setNewConnectionCallback(std::bind(&newConnCb,_1));
	client.setWriteCallback(std::bind(&writeCb,_1));
	client.setCloseCallback(std::bind(&closeCb,_1));
	client.setMessageCallback(std::bind(&messageCb,_1,_2));
	client.connect();
 	printf("File: HttpClientTest, main function, Main Thread Loop Start.\n");
 	loop.loop();
 	return 0;
}
