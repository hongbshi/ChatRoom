//Not Thread Safe

#ifndef ChatRoom_Buffer_H
#define ChatRoom_Buffer_H

#include<vector>
#include<string>
namespace ChatRoom
{
	class Buffer
	{
	public:
		//Construct
		Buffer() :readIndex_(0),writeIndex_(0) 
		{
			store_ = std::vector<char>(256);
		}
		//Original data will be cleared  
		int Read(std::string& des);
		//Append to Buffer
		int Read(Buffer& des);
		//Clear original data
		void Read(std::string& des, int& len);
		//Append to Buffer
		void Read(Buffer& des, int& len);
		ssize_t ReadSocket(int sockfd,int* Errno);
		ssize_t WriteSocket(int sockfd, int* Errno);
		//Write to Buffer
		void Append(const std::string& src);
		void Append(const Buffer& src);
		void Append(const char* src);
		//other Function
		int Readable() const;
		int Writable() const;
		std::string getString() const;
		std::string getStringLen(int& len) const;
		char* getBegin();
		void clear();
	private:
		void adjustStore();
		int readIndex_;
		int writeIndex_;
		std::vector<char> store_;
	};
}
#endif
