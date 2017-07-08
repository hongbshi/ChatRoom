#include "HttpRequest.h"
#include "Buffer.h"


using namespace ChatRoom;

void HttpRequest::appendToBuff(Buffer& buff){
	char tmp[128];
	//
	snprintf(tmp, sizeof tmp,
		"%s %s %s\r\n", 
		getStringMethod().c_str(), 
		getUrl().c_str(),
		getStringVersion().c_str());
	buff.writeToBuffer(tmp);
	//
	for (auto it = headers_.begin(); it != headers_.end(); ++it) {
		snprintf(tmp, sizeof tmp,
			"%s: %s\r\n", it->first.c_str(), it->second.c_str());
		buff.writeToBuffer(tmp);
	}
	buff.writeToBuffer("\r\n");
	//Send the body
	buff.writeToBuffer(body_);
	buff.writeToBuffer("\r\n");
}