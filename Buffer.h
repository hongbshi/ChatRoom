//Not Thread Safe
#ifndef ChatRoom_Buffer_H
#define ChatRoom_Buffer_H
#include <vector>
#include <string>
namespace ChatRoom
{
	class Buffer
	{
		//Read from Buffer and write/append to buffer
	public:
		//Construct
		Buffer() :readIndex_(0),
			writeIndex_(0),
			store_(std::vector<char>(256)) {}
		//Clear the dst
		int read(std::string& dst);
		int read(char* dst,int len);
		int append(const char* src);
		int append(const std::string& src);
		int append(Buffer& src); //src will be cleared
		//
		const char* readCRLF();
		void retrival(const char* str) {
			char* start = getBegin();
			readIndex_ += (str - start);
			adjustStore();
		}
		//
		ssize_t readSocket(int sockfd, int* Errno);
		ssize_t writeSocket(int sockfd, int* Errno);
		ssize_t writeFile(const char* path, int* Errno);
		//other Function
		std::string getString() const;
		std::string getStringLen(int& len) const;
		int readable() const { return writeIndex_ - readIndex_; }
		int writable() const { return store_.capacity() - writeIndex_ - 1; }
		void clear() { readIndex_ = writeIndex_ = 0; }
		bool isEmpty() { return readable();}
	private:
		void adjustStore();
		int append(const char* src, int len);
		char* getBegin() const { 
			return &store_[readIndex_]; 
		}
		char* getEnd() const { 
			return &store_[writeIndex_];
		}
		int readIndex_;
		int writeIndex_;
		std::vector<char> store_;
	};
}
#endif
