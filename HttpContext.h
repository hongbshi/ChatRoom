#ifndef  ChatRoom_HttpContext_H
#define  ChatRoom_HttpContext_H
#include <string>
#include <unordered_map>
#include "HttpRequest.h"
namespace ChatRoom
{
	using std::string;
	class Buffer;
	class HttpContext {
	public:
		enum HttpParseState {
			kExpectedRequestLine,
			kExpectedHeadLine,
			kExpectedBody,
			kCompleted
		};
		HttpContext() :state_(kExpectedRequestLine) {}
		bool getAll()const { state_ == kCompleted; }
		HttpRequest& getRequest() { return request_; }
		const HttpRequest& getRequest() const{ return request_; }
		void parseRequest(Buffer *buff);
		void resetRequest() {
			HttpRequest tmp;
			request_.swap(tmp);
			state_ = kExpectedRequestLine;
		}
	private:
		bool parseRequestLine(const char *start, const char *end);
		HttpParseState state_;
		HttpRequest request_;
	};
}
#endif // ! ChatRoom_HttpContext_H
