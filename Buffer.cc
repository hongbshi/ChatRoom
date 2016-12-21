#include "Buffer.h"
#include "Socket.h"
#include<cstring>
using namespace ChatRoom;
int Buffer::Read(std::string& des)
{
	des.clear();
	des += std::string(&store_[readIndex_], &store_[writeIndex_]);
	int len = Readable();
	clear();
	return len;
}

int Buffer::Read(Buffer& des)
{
	des.Append(getString());
	int len = Readable();
	clear();
	return len;
}

void Buffer::Read(std::string& des, int& len)
{
	des.clear();
	int size = Readable();
	if (len > size)
		len = Read(des);
	else
	{
		des += std::string(&store_[readIndex_], &store_[readIndex_ + len]);
		readIndex_ += len;
		adjustStore();
	}
}

void Buffer::Read(Buffer& des, int& len)
{
	int size = Readable();
	if (len > size)
		len = Read(des);
	else
	{
		des.Append(getStringLen(len));
		readIndex_ += len;
		adjustStore();
	}
}

ssize_t ChatRoom::Buffer::ReadSocket(int sockfd,int* Errno)
{
	//Initial variable
	int len1 = Writable()-1;
	char* start1 = &store_[writeIndex_];
	int len2 = 65535;
	char start2[65536];
	struct iovec iov[2];
	iov[0].iov_base = start1;
	iov[0].iov_len = len1;
	iov[1].iov_base = start2;
	iov[1].iov_len = len2;
	//Get read result
	ssize_t result = readvSocket(sockfd, iov, 2);
	//Deal result
	if (result > 0)
	{
		if (result <= len1)
			writeIndex_ += result;
		else
		{
			writeIndex_ += len1;
			Append(start2);
		}
	}
	else if (result < 0)
		*Errno = errno;
	return result;
}

ssize_t ChatRoom::Buffer::WriteSocket(int sockfd, int * Errno)
{
	ssize_t result=writeSocket(sockfd, &store_[readIndex_], Readable());
	if (result < 0)
		*Errno = errno;
	else if (0 < result)
		readIndex_ += result;
	return result;
}

void Buffer::Append(const std::string& s)
{
	int len = s.size();
	int size = Writable();
	if (size < len)
	{
		int cap = store_.capacity();
		store_.resize(2 * cap);
	}
	memcpy(&store_[writeIndex_], &*s.begin(), len);
	writeIndex_ += len;
}

void Buffer::Append(const Buffer& s)
{
	Append(s.getString());
}

void Buffer::Append(const char* s)
{
	if (s == nullptr)
		return;

	int len = strlen(s);
	int size = Writable();
	if (size < len)
	{
		int cap = store_.capacity();
		store_.resize(2 * cap);
	}
	memcpy(&store_[writeIndex_], s, len);
	writeIndex_+=len;
}

int Buffer::Readable() const
{
	return writeIndex_-readIndex_;
}

int ChatRoom::Buffer::Writable() const
{
	return store_.capacity() - writeIndex_;
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
	if (len < Readable())
		return std::string(&store_[readIndex_], &store_[readIndex_ + len]);
	else
	{
		len = Readable();
		return std::string(&store_[readIndex_], &store_[writeIndex_]);
	}
}

char * ChatRoom::Buffer::getBegin()
{
	return &*store_.begin();
}

void Buffer::clear()
{
	readIndex_ = writeIndex_ = 0;
}

void ChatRoom::Buffer::adjustStore()
{
	int len = Readable();
	if (2 * len < readIndex_)
	{
		memmove(&store_[0], &store_[readIndex_], Readable());
		readIndex_ = 0;
		writeIndex_ = len;
	}
}
