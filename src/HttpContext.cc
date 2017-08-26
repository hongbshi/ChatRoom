#include "HttpContext.h"
#include "Buffer.h"

void ChatRoom::HttpContext::parseRequest(Buffer *buff)
{
	bool hasMore = true;
	while(hasMore){
		//std::string str = buff->getString();
		//printf("%s\n", str.c_str());
		if (state_ == kCompleted) break;
		const char *crlf = buff->readCRLF();
		if (crlf == nullptr) {
			hasMore = false;
			break;
		}
		const char *start = buff->getBegin();
		if (state_ == kExpectedRequestLine) {
			//if (!parseRequestLine(start, crlf)) resetRequest();
			//else state_ = kExpectedHeadLine;
			if (parseRequestLine(start, crlf)) state_ = kExpectedHeadLine;
			else hasMore = false;
		}
		else if (state_ == kExpectedHeadLine) {
			//if (crlf == start) state_ = kExpectedBody;
			if (crlf == start) state_ = kCompleted;
			else request_.addHeader(start, crlf);
		}
		else if (state_ == kExpectedBody) {
			request_.setBody(start, crlf);
			state_ = kCompleted;
		}
		buff->retrival(crlf + 2);
	}
	std::string str = buff->getString();
	printf("%s\n", str.c_str());
}

bool ChatRoom::HttpContext::parseRequestLine(const char * start, const char * end)
{
	//Find first space
	const char *s1 = start;
	while (s1 < end && *s1 != ' ') ++s1;
	if (s1 >= end) return false;
	//Find second space
	const char *s2 = s1 + 1;
	while (s2 < end && *s2 != ' ') ++s2;
	if (s2 >= end) return false;
	//Write to HttpRequest
	request_.setMethod(start, s1);
	request_.setUrl(s1 + 1, s2);
	request_.setVersion(s2 + 1, end);
	return true;
}
