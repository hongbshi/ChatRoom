#include <iostream>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <stdlib.h>

//#include "TcpProxyClient.h"
//#include "TcpProxyServer.h"
//#include "Thread.h"
//#include "HttpServer.h"
//#include "EventLoop.h"
//#include "HttpRequest.h"
//#include "HttpResponse.h"
//
//using namespace std;
//using namespace ChatRoom;
//using namespace std::placeholders;
//
//typedef HttpResponse::HttpResponseStatus HttpResponseStatus;
//
using namespace std;
bool convertAddr(const char *ip, const char *port, struct sockaddr_in & addr){
	bzero(&addr, sizeof addr);
	addr.sin_family = AF_INET;
	unsigned short pt = atoi(port);
	addr.sin_port = htons(pt);
	int ans = inet_pton(AF_INET, ip, &addr.sin_addr); 
	if(ans < 1) return false;
	return true;
}

bool reconvertAddr(char *ip, int iplen, char *port, int portlen, const struct sockaddr_in & addr){
	unsigned int pt = ntohs(addr.sin_port);
	snprintf(port, portlen, "%u", pt);
	const char *ans = inet_ntop(AF_INET, (void*)(&addr.sin_addr), ip, iplen);
	if(ans == nullptr) return false;
	return true;
}

char* parseBuff(char *buff, int len, char *ip, int iplen, char *port, int portlen, bool flag){
	char *lineEnd = buff, *space = buff, *end = buff + len;
	while(space < end && *space != ' ') ++space;
	if(space == end) return nullptr;
	lineEnd = space + 1;
	while(lineEnd < end && *lineEnd != '\n') ++lineEnd;
	if(lineEnd == end && !flag) return nullptr;
	char *tmp = buff;
	while(tmp < space) {
		*ip = *tmp;
		++ip, ++tmp;
	}
	*ip = '\0';
	++tmp;
	while(tmp < lineEnd){
		if('0' <= *tmp && *tmp <= '9') {
			*port = *tmp;
			++port;
		}
		++tmp;
	}
	*port = '\0';
	if(lineEnd < end && *lineEnd == '\n') ++lineEnd;
	return lineEnd;
}

int main(int argc,char *argv[]){
	int fd = open("./AddrConf.txt", O_RDONLY);
	if(fd < 0){
		printf("File: TcpProxyServerTest2.cc, main function, open AddrConf.txt failed.\n");
		return -1;
	}
	int bufflen = 256, ans = 0, cnt = 0;
	char buff[256], ip[32], port[16];
	while(ans = read(fd, buff, bufflen) > 0){

	}
	if(ans < 0){
		printf("File: TcpProxyServerTest2, main function, read AddrConf.txt error!\n");
		return -1;
	}
	
	
	
	
	
	
	
    //	
 	printf("File: TcpProxyServerTest.cc, main funcion, main thread id is %d.\n", getCurrentThreadTid());
	EventLoop loop;
	TcpProxyServer server(&loop,listenAddr,serverAddr);
	server.start();
 	loop.loop();
 	return 0;
}

