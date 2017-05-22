#include "Buffer.h"
#include "Socket.h"
#include <cstring>
#include <algorithm>
#include <sys/types.h>    
#include <sys/stat.h>   
#include <fcntl.h>
using namespace ChatRoom;
int Buffer::readFromBuffer(std::string& str)
{
	str.assign(&store_[readIndex_], &store_[writeIndex_]);
	int len = writeIndex_ - readIndex_;
	readIndex_ = writeIndex_ = 0;
	return len;
}

int ChatRoom::Buffer::readFromBuffer(char *dst,int len)
{
	if (len < 1) return 0;
	int result = readable();
	int real = (len - 1 <= result) ? len - 1 : result;
	strncpy(dst, &store_[readIndex_], real);
	dst[real] = '\0';
	readIndex_ += real;
	adjustStore();
	return real;
}

int Buffer::writeToBuffer(const std::string& src)
{
	const char *ptr = (src.empty() ? nullptr : &src[0]);
	return writeToBuffer(ptr, src.size());
}

int Buffer::writeToBuffer(Buffer& src)
{
	int result = writeToBuffer(src.getBegin(), src.readable());
	src.clear();
	return result;
}

const char * ChatRoom::Buffer::readCRLF()
{
	const char* start = getBegin();
	const char* end = getEnd();
	while (start < end){
		if (*start == '\r'&& start + 1 < end && *(start + 1) == '\n') break;
		else ++start;
	}
	if (start + 1 < end && *start == '\r') return start;
	else return nullptr;
}

std::string ChatRoom::Buffer::readUntilCRLF(const char* ptr)
{	
	std::string result;
	if (ptr == nullptr) return result;
	result.assign(getBegin(), ptr + 2);
	retrival(ptr + 2);
	adjustStore();
	return result;
}

int Buffer::writeToBuffer(const char* src)
{
	if (src == nullptr) return 0;
	else return writeToBuffer(src, strlen(src));
}

int ChatRoom::Buffer::writeToBuffer(const char * src, int len)
{
	if (src == nullptr) return 0;
	if (writable() < len) store_.resize(2 * store_.capacity());
	memcpy(&store_[writeIndex_], src, len);
	writeIndex_ += len;
	return len;
}

ssize_t ChatRoom::Buffer::readSocket(int sockfd,int* Errno)
{
	//Initial variable
	int len1 = writable();
	char* start1 = &store_[writeIndex_];
	int len2 = 65535;
	char start2[65536];
	struct iovec iov[2];
	iov[0].iov_base = start1;
	iov[0].iov_len = len1;
	iov[1].iov_base = start2;
	iov[1].iov_len = len2;
	//Get read result
	ssize_t result = readvFromSocket(sockfd, iov, 2);
	//Deal result
	if (result > 0)
	{
		if (result <= len1)
			writeIndex_ += result;
		else
		{
			writeIndex_ += len1;
			writeToBuffer(start2,result-len1);
		}
	}
	else if (result < 0)
		*Errno = errno;
	return result;
}

ssize_t ChatRoom::Buffer::writeSocket(int sockfd, int * Errno)
{
	ssize_t result=writeToSocket(sockfd, 
		&store_[readIndex_], 
		readable());
	if (result < 0) *Errno = errno;
	else if (0 < result) readIndex_ += result;
	return result;
}

ssize_t ChatRoom::Buffer::readFromFile(const char * path, int * Errno)
{
	////File not exit
	struct stat st;
	if (stat(path, &st) == -1)
	{
		*Errno = errno;
		return -1;
	}
	//File cannot open
	int fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		*Errno = errno;
		return -1;
	}
	//File can open
	ssize_t result = 0;
	while (1)
	{
		int len1 = writable();
		char* start1 = &store_[writeIndex_];
		int len2 = 65535;
		char start2[65536];
		struct iovec iov[2];
		iov[0].iov_base = start1;
		iov[0].iov_len = len1;
		iov[1].iov_base = start2;
		iov[1].iov_len = len2;
		ssize_t tmp = readv(fd, iov, 2);
		if (tmp < 0)
		{
			writeIndex_ -= result;
			result = -1;
			*Errno = errno;
			break;
		}
		if (tmp == 0) break;
		//Deal result
		if (tmp <= len1)
		{
			writeIndex_ += tmp;
			result += tmp;
			break;
		}
		else
		{
			writeIndex_ += len1;
			writeToBuffer(start2,tmp-len1);
			result += tmp;
		}	
	}
	return result;
}

std::string Buffer::getString() const
{
	return std::string(&store_[readIndex_], &store_[writeIndex_]);
}

std::string ChatRoom::Buffer::getStringLen(int& len) const
{
	if (len < 1)
	{
		len = 0;
		return std::string();
	}
	int result = (len < readable()) ? len : readable();
	len = result;
	return std::string(&store_[readIndex_], &store_[readIndex_ + len]);
}

void ChatRoom::Buffer::adjustStore()
{
	int len = readable();
	if (2 * len < readIndex_)
	{
		memmove(&store_[0], &store_[readIndex_], len);
		readIndex_ = 0;
		writeIndex_ = len;
	}
}
