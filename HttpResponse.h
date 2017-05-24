#ifndef  ChatRoom_HttpResponse_H
#define  ChatRoom_HttpResponse_H
#include <string>
#include <unordered_map>
namespace ChatRoom
{
	class Buffer;
	using std::string;
	typedef ChatRoom::HttpRequest::HttpVersion;
	class HttpResponse {
	public:
		enum HttpResponseStatus {
			kUnKnown,
			k200Succeed = 200,
			k400Failed = 400,
			k404NotFound = 404
		};
		HttpResponse() :statusCode_(kUnKnown) {}
		void setHttpVersion(string && ver) { httpVersion_ = ver; }
		void setStatus(HttpResponseStatus && code) {
			statusCode_ = code; 
			switch (code){
			case k200Succeed:
				statusMessage_ = "200 Succeed";
			case k400Failed:
				statusMessage_ = "400 Failed";
			case k404NotFound:
				statusMessage_ = "404 Not Found";
			default:
				statusMessage_ = "500 UnKnown";
			}
		}
		string getStatus() { 
			return statusMessage_; 
		}
		//void setStatusMessage(string && mess);
		void setHeader(std::unordered_map<string, string> && head) { header_ = head; }
		void setBody(string && body) { body_ = body; }
		void setFilePath(string && path) { filePath_ = path; }
		void appendToBuffer(Buffer& buff);
	private:
		HttpVersion httpVersion_;
		HttpResponseStatus statusCode_;
		string statusMessage_;
		std::unordered_map<string, string> header_;
		string filePath_;
		string body_;
	};
}
#endif // ! ChatRoom_HttpResponse_H
