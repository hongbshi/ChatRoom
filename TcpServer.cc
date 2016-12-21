#include "TcpServer.h"
#include "TcpConnection.h"

void ChatRoom::TcpServer::newConnection(int sockfd, sockaddr * clientAddr)
{

	//std::shared_ptr<TcpConnection> conn=std::make_shared<TcpConnection>(sockfd,clientAddr,);
	//conn->setxxxcallback();
	//
	//

	//loop->runInloop(std::bind(&TcpConnection::conncetedEstablished,this));
}
