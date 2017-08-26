#include "ReadAddr.h"

using namespace ChatRoom;

bool ReadAddr::convertAddr(const char *ip, const char *port, struct sockaddr_in & addr){
	bzero(&addr, sizeof addr);
	addr.sin_family = AF_INET;
	unsigned short pt = atoi(port);
	addr.sin_port = htons(pt);
	int ans = inet_pton(AF_INET, ip, &addr.sin_addr); 
	if(ans < 1) return false;
	return true;
}

bool ReadAddr::reconvertAddr(char *ip, int iplen, char *port, int portlen, const struct sockaddr_in & addr){
	unsigned int pt = ntohs(addr.sin_port);
	snprintf(port, portlen, "%u", pt);
	const char *ans = inet_ntop(AF_INET, (void*)(&addr.sin_addr), ip, iplen);
	if(ans == nullptr) return false;
	return true;
}

char* ReadAddr::parseOne(char *start, int len, char *ip, int iplen, char *port, int portlen, bool flag){
	char *end = start + len, *space = start, *ans = nullptr;
	while(space < end && *space != ' ') ++space;
	if(space == end) return ans;
	ans = space + 1;
	while(ans < end && *ans != '\n') ++ans;
	if(ans == end && !flag) return nullptr;
	char *tmp = start;
	while(tmp < space) {
		if(('0' <= *tmp && *tmp <= '9') || *tmp == '.') *ip++ = *tmp;
		++tmp;
	}
	*ip = '\0';
	while(tmp < ans){
		if('0' <= *tmp && *tmp <= '9') 	*port++ = *tmp;
		++tmp;
	}
	*port = '\0';
	if(ans < end && *ans == '\n') ++ans;
	return ans;
}

bool ReadAddr::parseBuff(char *start, int len, bool flag, char* & pos, std::vector<struct sockaddr_in> & addr){
	//
	if(len < 0) return false;
	if(len == 0){
		pos = start;
		return true;
	}
	//
	char ip[32], port[16];
	struct sockaddr_in serAddr;
	int iplen = 32, portlen = 16;
	char *end = start + len;
	pos = start;
	char *next = parseOne(pos, end - pos, ip, iplen, port, portlen, flag); 
	while(next){
		if(!convertAddr(ip, port, serAddr)) return false; 
		addr.emplace_back(serAddr);
		pos = next;
		next = parseOne(pos, end - pos, ip, iplen, port, portlen, flag); 
	}
	return true;
}

bool ReadAddr::readServerAddr(const char *str, std::vector<struct sockaddr_in> & addr){
	//open file
	int fd = open(str, O_RDONLY);
	if(fd < 0) return false;
	//read file
	int readlen;
	char buff[512], *buffEnd = buff + sizeof buff, *start = buff, *pos, *end;
	//
	while((readlen = read(fd, start, buffEnd - start)) > 0){
		end = start + readlen; start = buff;
		if(!parseBuff(start, end - start, false, pos, addr)) return false;
		while(pos < end) *start++ = *pos++;
	}
	if(readlen < 0) return false;
	else return parseBuff(buff, start - buff, true, pos, addr);
}
