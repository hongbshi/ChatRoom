#include "HttpResponse.h"
#include "Buffer.h"
void ChatRoom::HttpResponse::appendToBuffer(Buffer * buff)
{
	char response[128];
	//Send the status lines
	snprintf(response, sizeof response,
		"HTTP/%s %s\r\n", httpVersion_.c_str(), getStatus().c_str());
	buff->append(response);
	//Send the header
	for (auto it = header_.begin(); it != header_.end(); ++it) {
		snprintf(response, sizeof response,
			"%s:%s\r\n", it->first.c_str(), it->second.c_str());
		buff->append(response);
	}
	//Send the body
	buff->append("\r\n");
	int Errno=0;
	if (!filePath_.empty()) {
		buff->writeFile(filePath_.c_str(), &Errno);
		//if(Errno)
	}
	buff->append(body_);
}
