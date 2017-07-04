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

void connCb(int sockfd){
	std::string name{"client TcpConnection"}; 
	struct sockaddr localAddr = getLocalAddr(sockfd);
	struct sockaddr peerAddr = getPeerAddr(sockfd);
	TcpConnectionPtr ptr = make_shared(TcpConnection(loop,sockfd,localAddr,peerAddr,name));
	ptr->setConnectedCallback();
	ptr->setCloseCallback();
	ptr->setWriteCallback();
	ptr->setMessageCallback();
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
 	if(result < 1) printf("Main function error! ip or port unavaliable.\n");
 	//Initial variable
 	EventLoop loop;
 	Connector connector(&loop,serverAddr);
 	connector.setConnectionCallback(connCb);
 	connector.start();
 	return 0;
}

