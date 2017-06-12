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
		int readFromBuffer(std::string & src);
		int readFromBuffer(char *dst,int len);
		int writeToBuffer(const char *src);
		int writeToBuffer(const std::string& src);
		int writeToBuffer(Buffer& src); //src will be cleared
		//
		const char* readCRLF () const;
		std::string readUntilCRLF(const char *ptr);
		void retrival(const char* str) {
			const char* start = getBegin();
			readIndex_ += (str - start);
			adjustStore();
		}
		//
		ssize_t readSocket(int sockfd, int* Errno);
		ssize_t writeSocket(int sockfd, int* Errno);
		ssize_t readFromFile(const char* path, int* Errno);
		//other Function
		std::string getString() const;
		std::string getStringLen(int & len) const;
		int readable() const { return writeIndex_ - readIndex_; }
		int writable() const { return store_.capacity() - writeIndex_ - 1; }
		void clear() { readIndex_ = writeIndex_ = 0; }
		bool isEmpty() const { return readIndex_ == writeIndex_;}

		const char* getBegin() const {
			return static_cast<const char*>(&store_[readIndex_]);
		}
		const char* getEnd() const {
			return static_cast<const char*>(&store_[writeIndex_]);
		}
	private:
		void adjustStore();
		int writeToBuffer(const char *src, int len);
		int readIndex_;
		int writeIndex_;
		std::vector<char> store_;
	};
}
#endif
