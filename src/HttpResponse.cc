#include "HttpResponse.h"
#include "Buffer.h"
void ChatRoom::HttpResponse::appendToBuffer(Buffer & buff)
{
	char response[128];
	//Send the status lines
	snprintf(response, sizeof response,
		"%s %s\r\n", 
		getStringHttpVersion().c_str(), 
		//getStringStatusCode().c_str(),
		statusMessage_.c_str());
	buff.writeToBuffer(response);
	//Send the header
	snprintf(response, sizeof response, "Content-Length: %zd\r\n", body_.size());
    buff.writeToBuffer(response);
    buff.writeToBuffer("Connection: Keep-Alive\r\n");
	for (auto it = header_.begin(); it != header_.end(); ++it) {
		snprintf(response, sizeof response,
			"%s: %s\r\n", it->first.c_str(), it->second.c_str());
		buff.writeToBuffer(response);
	}
	buff.writeToBuffer("\r\n");
	//Send the body
	buff.writeToBuffer(body_);
	buff.writeToBuffer("\r\n");
}

void ChatRoom::HttpResponse::appendFileToBuffer(Buffer& buff)
{
	int Errno;
	if (!filePath_.empty()) {
		buff.readFromFile(filePath_.c_str(), &Errno);
		//if(Errno)
	}
}
