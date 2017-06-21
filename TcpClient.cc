#include "TcpClient.h"
#include "Socket.h"

using namespace ChatRoom;

TcpClient::TcpClient(EventLoop *loop, const struct sockaddr_in & server):loop_(loop),serverAddr_(server){
	sockfd_ = createNonblockSocket(AF_INET,SOCK_STREAM,0);
}

void TcpClient::conn(){
	if(connectAddress(sockfd_, sockaddr_cast(&serverAddr_), sizeof(serverAddr_)) < 0){

	}
	else{
		isconn_ = true;
		if(connCb_)
	}
}

void TcpClient::disconn(){

}