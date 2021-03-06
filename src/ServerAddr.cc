#include "ServerAddr.h"
#include <cstdlib>
#include <cstring>

using namespace ChatRoom;

bool ServerAddr::addServerAddr(const char *ip, const char *port){
	//create new sockaddr_in
	struct sockaddr_in tmp;
	bzero(&tmp, sizeof(tmp));                                                              
	//Write sin_family
	tmp.sin_family = AF_INET;
	//Write port
	unsigned short pt = atoi(port);
	tmp.sin_port = htons(pt);
	//Write ip
	int result = inet_pton(AF_INET, ip, &tmp.sin_addr);
	if(result <= 0) return false; 
	//
	serverAddrPool.addr_.emplace_back(std::move(tmp));
	return true;
}

void ServerAddr::swapAddr(std::vector<struct sockaddr_in> & addr){
	serverAddrPool.addr_.swap(addr);
}

bool ServerAddr::getNext(struct sockaddr_in & addr){
	if(serverAddrPool.addr_.empty()) return false;
	++serverAddrPool.idx_;
	serverAddrPool.idx_ %= serverAddrPool.addr_.size();
	addr = serverAddrPool.addr_[serverAddrPool.idx_];
	return true;
}

//static int ServerAddr::getNum() const{
//	return serverAddrPool.addr_.size();
//}

ServerAddr::ServerAddr():idx_(0),addr_(){}

ServerAddr::~ServerAddr(){}

ServerAddr ServerAddr::serverAddrPool;

