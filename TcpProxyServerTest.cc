#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <vector>

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

using namespace std;

int main(int argc,char *argv[]){
 	printf("File: TcpProxyServerTest.cc, main funcion, main thread id is %d.\n", getCurrentThreadTid());
	EventLoop loop;
	std::string addrConf = "./AddrConf.txt";
	TcpProxyServer server(&loop, addrConf);
	server.start();
	printf("File: TcpProxyServerTest.cc, main function, main loop start.\n");
 	loop.loop();
 	return 0;
}

