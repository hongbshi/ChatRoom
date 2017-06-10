#include <iostream>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <arpa/inet.h>

#include "Thread.h"
#include "HttpServer.h"
#include "EventLoop.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "TcpConnection.h"

using namespace std;
using namespace ChatRoom;
using namespace std::placeholders;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef HttpResponse::HttpResponseStatus HttpResponseStatus;

void thrdInitial(EventLoop *loop){
	printf("%s\n", "Thread initial. HttpServerTest.cc, thrdInitial function.");
}

void newCb(TcpConnectionPtr ptr){
	printf("%s Connection coming. HttpServerTest.cc, newCb function.\n", ptr->getName().c_str());
}

void messCb(const HttpRequest& request, HttpResponse& response){
	printf("HttpServerTest messCb, File: HttpServerTest.cc, newCb function.\n");
	response.setHttpVersion("HTTP/1.1");
	HttpResponseStatus status = HttpResponseStatus::k200Succeed;
	response.setStatus(status);
	std::string str("<html><head><title>Test</title></head><body><p>body First time</p><p>title HeHe</p></body></html>");
	response.setBody(str);
}

void closeCb(TcpConnectionPtr ptr){
	printf("%s Connection closed. HttpServerTest.cc, closeCb function.\n", ptr->getName().c_str());
}

int main(int argc,char *argv[]){
	for(int i = 0;i < argc; ++i) cout << argv[i] <<endl;
	if(argc != 3) {
		printf("%s\n", "<Ip> <Port>");
		return -1;
	}
	struct sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(serverAddr));
	int port = atoi(argv[2]);
	printf("Listen port is %d. File : HttpServerTest.cc, main funcion.\n", port);
  	serverAddr.sin_family = AF_INET;
  	serverAddr.sin_port = htons(port);
 	int result = inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);
 	if(result == 0) printf("Invalid address! File : HttpServerTest.cc, main funcion.\n");
 	if(result == -1) printf("inet_pton function error! File : HttpServerTest.cc, main funcion.\n");
 	EventLoop loop;
 	printf("Current Thread is %d, File : HttpServerTest.cc, main funcion.\n", getCurrentThreadTid());
 	HttpServer httpServ(&loop,&serverAddr,false);
 	httpServ.setThreadInitialCallback(std::bind(&thrdInitial,_1));
 	httpServ.setNewConnectionCallback(std::bind(&newCb,_1));
 	httpServ.setMessageCallback(std::bind(&messCb,_1,_2));
 	httpServ.setCloseCallback(std::bind(&closeCb,_1));
 	httpServ.setThreadNumber(1);
 	httpServ.start();
 	loop.loop();
 	return 0;
}

