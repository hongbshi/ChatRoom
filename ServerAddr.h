#ifndef ChatRoom_ServerAddr_H
#define ChatRoom_ServerAddr_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <vector>

namespace ChatRoom{
	//Not thread safe, only used in the main thread to intial server address.
	class ServerAddr{
		public:
			static bool addServerAddr(const char *ip, const char *port);
			static bool getNext(struct sockaddr_in & addr) const;
			//static int getNum() const;
		private:
			ServerAddr();
			~ServerAddr();

			int idx_;
			std::vector<struct sockaddr_in> addr_;
			static ServerAddr serverAddrPool;
	};
}
#endif
