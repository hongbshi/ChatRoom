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
		Buffer() :readIndex_(0),writeIndex_(0) {}
		//Original data will be cleared  
		int Read(std::string& des);
		//Append to Buffer
		int Read(Buffer& des);
		//Clear original data
		void Read(std::string& des, int& len);
		//Append to Buffer
		void Read(Buffer& des, int& len);
		//Write to Buffer
		void Append(const std::string& src);
		void Append(const Buffer& src);
		void Append(const char* src);
		//other Function
		int getLen() const;
		std::string getString() const;
		void clear();
	private:
		std::vector<char> store_;
		int readIndex_;
		int writeIndex_;
	};
}
#endif
