#include <iostream>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <arpa/inet.h>

#include "HttpServer.h"
#include "EventLoop.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

using namespace std;
using namespace ChatRoom;
using namespace std::placeholders;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
void thrdInitial(EventLoop *loop){

}

void newCb(TcpConnectionPtr ptr){

}

void messCb(const HttpRequest&, HttpResponse&){

}

void closeCb(TcpConnectionPtr ptr){

}

int main(int argc,char *argv[]){
	for(int i = 0;i < argc; ++i) cout << argv[i] <<endl;
	if(argc != 3) {
		printf("%s\n", "<Ip> <Port>");
		return -1;
	}
	int port = atoi(argv[2]);
	struct sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(serverAddr));
  	serverAddr.sin_family = AF_INET;
  	serverAddr.sin_port = htons(port);
 	inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);
 	EventLoop loop;
 	HttpServer httpServ(&loop,&serverAddr,false);
 	httpServ.setThreadInitialCallback(std::bind(&thrdInitial,_1));
 	httpServ.setNewConnectionCallback(std::bind(&newCb,_1));
 	httpServ.setMessageCallback(std::bind(&messCb,_1,_2));
 	httpServ.setCloseCallback(std::bind(&closeCb,_1));
 	httpServ.start();
 	loop.loop();
 	return 0;
}

