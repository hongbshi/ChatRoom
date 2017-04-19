#ifndef  ChatRoom_HttpContext_H
#define ChatRoom_HttpContext_H
#include<string>
#include<unordered_map>
namespace ChatRoom
{
	using std::string;
	class HttpContext {
	public:
		enum HttpRequestStatus {
			kWaitRequestLine,
			kWaitHeadersLine,
			kWaitBody,
			kErrno
		};
		HttpContext() :status_(kWaitRequestLine) {}
		HttpRequestStatus getStatus() { return status_; }
	private:
		HttpRequestStatus status_;
		string method_;
		string url_;
		string version_;
		std::unordered_map<string, string> headers_;
		string body_;
	};
}
#endif // ! ChatRoom_HttpContext_H
