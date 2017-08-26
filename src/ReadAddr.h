#ifndef  ChatRoom_ReadAddr_H
#define  ChatRoom_ReadAddr_H

#include <fcntl.h>
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

namespace ChatRoom
{
	class ReadAddr{
	public:
		static bool convertAddr(const char *ip, const char *port, struct sockaddr_in & addr);	
		static bool reconvertAddr(char *ip, int iplen, char *port, int portlen, const struct sockaddr_in & addr);
		static bool readServerAddr(const char *str, std::vector<struct sockaddr_in> & addr);
	private:
		static char* parseOne(char *start, int len, char *ip, int iplen, char *port, int portlen, bool flag);	
		static bool parseBuff(char *start, int len, bool flag, char* &pos, std::vector<struct sockaddr_in> & addr);
	};
}
#endif // ! ChatRoom_ReadAddr_H
