#include <iostream>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <arpa/inet.h>

#include "TcpProxyClient.h"
#include "TcpProxyServer.h"
#include "Thread.h"
#include "HttpServer.h"
#include "EventLoop.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

using namespace std;
using namespace ChatRoom;
using namespace std::placeholders;

typedef HttpResponse::HttpResponseStatus HttpResponseStatus;

int main(int argc,char *argv[]){
	//for(int i = 0;i < argc; ++i) cout << argv[i] <<endl;
	if(argc != 5) {
		printf("%s\n", "<Tomcat Ip> <Tomcat Port> <Listen Ip> <Listen port>");
		return -1;
	}
	struct sockaddr_in serverAddr, listenAddr;
	bzero(&serverAddr, sizeof(serverAddr));
	bzero(&listenAddr, sizeof(listenAddr));
	int port1 = atoi(argv[2]), port2 = atoi(argv[4]);
	//
  	serverAddr.sin_family = AF_INET;
  	serverAddr.sin_port = htons(port1);
 	int result = inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);
 	if(result <= 0) printf("File : TcpProxyServerTest.cc, main funcion, tomcat server address error.\n");
	//
  	listenAddr.sin_family = AF_INET;
  	listenAddr.sin_port = htons(port2);                           	
 	result = inet_pton(AF_INET, argv[3], &listenAddr.sin_addr); 	
 	if(result <= 0) printf("File : HttpServerTest.cc, main funcion, listen address error.\n");
	//
 	printf("File: TcpProxyServerTest.cc, main funcion, main thread id is %d.\n", getCurrentThreadTid());
	EventLoop loop;
	TcpProxyServer server(&loop,listenAddr,serverAddr);
	server.start();
 	loop.loop();
 	return 0;
}

