#include "Buffer.h"

using namespace ChatRoom;
int Buffer::Read(std::string& des)
{
	des.clear();
	int len = getLen();
	if (len > 0)
		for (int i = 0; i < len;i++)
			des.push_back(store_[i]);

	clear();
	return len;
}

int Buffer::Read(Buffer& des)
{
	des.Append(getString());
	int len = getLen();
	clear();
	return len;
}

void Buffer::Read(std::string& des, int& len)
{
	des.clear();
	int size = getLen();
	if (len > size)
	{
		len = Read(des);
		return;
	}
		
	for (int i = 0; i < len; i++)
	{
		des.push_back(store_[i]);
		readIndex_++;
	}	
		
}

void Buffer::Read(Buffer& des, int& len)
{
	int size = getLen();
	if (len > size)
		len = Read(des);
	else
	{
		std::string tmp = getString();
		des.Append(std::string(tmp.begin(), tmp.begin() + len));
		readIndex_ += len;
	}
}

void Buffer::Append(const std::string& s)
{
	int addLen = s.size();
	for (int i = 0; i < addLen;i++)
		store_.push_back(s[i]);
	writeIndex_ += addLen;
}

void Buffer::Append(const Buffer& s)
{
	Append(s.getString());
}

void Buffer::Append(const char* s)
{
	if (s == nullptr)
		return;

	int i = 0;
	while (s[i] != '\0')
	{
		store_.push_back(s[i]);
		i++, writeIndex_++;
	}
}

int Buffer::getLen() const
{
	return writeIndex_-readIndex_;
}

std::string Buffer::getString() const
{
	if (writeIndex_ == readIndex_)
		return std::string();

	int len = getLen();
	return std::string(&store_[readIndex_], len);
}

void Buffer::clear()
{
	readIndex_ = 0;
	writeIndex_ = 0;
}