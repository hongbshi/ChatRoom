#include<Buffer.h>

using namespace ChatRoom;

int Buffer::Read(std::string& d)
{
	d.clear();
	if (getSize() > 0)
		d.push_back(store_.begin(), store_.end());
	return getSize();
}

int Buffer::Read(Buffer& d)
{
	d.clear();
	d.Write(getString());
	return getSize();
}

void Buffer::Read(std::string& d, int& len)
{
	d.clear();
	if (len > getSize())
		len=Read(d);
	else
		d.push_back(store_.begin(), store_.begin() + len);
}

void Buffer::Read(Buffer& d, int& len)
{
	if (len > getSize())
		len = Read(d);
	else
	{
		std::string tmp = getString();
		d.Write(tmp.begin(), tmp.begin() + len);
	}
}


void Buffer::Write(const std::string& s)
{
	int addLen = s.size();
	store_.push_back(s.begin(), s.end());
	size += addLen;
}

void Buffer::Write(const Buffer& s)
{
	s.Write(s.getString());
}

void Buffer::Write(const char* s)
{
	int addLen = strlen(s);
	store_.push_back(s, s + addLen);
	size += addLen;
}

int Buffer::getSize()
{
	return size_;
}

std::string Buffer::getString()
{
	return std::string(store_.begin(), store_.end());
}

void Buffer::clear()
{
	store_.clear();
	size = 0;
}