#ifndef  ChatRoom_HttpResponse_H
#define  ChatRoom_HttpResponse_H
#include <string>
#include <unordered_map>
#include "HttpRequest.h"
namespace ChatRoom
{
	class Buffer;
	using std::string;
	class HttpResponse {
	public:
		enum HttpResponseStatus {
			kUnKnown,
			k200Succeed = 200,
			k400Failed = 400,
			k404NotFound = 404
		};

		typedef HttpRequest::HttpVersion HttpVersion;

		HttpResponse() :httpVersion_(HttpVersion::kUnknown), statusCode_(kUnKnown) {}

		void setHttpVersion(HttpVersion && ver) { httpVersion_ = ver; }

		void setHttpVersion(const char *start, const char *end) {
			std::string str(start, end);
			setHttpVersion(str);
		}

		void setHttpVersion(const std::string & str) {
			if (str == "HTTP/1.0") {
				httpVersion_ = HttpVersion::kHttp10;
			}
			else if (str == "HTTP/1.1") {
				httpVersion_ = HttpVersion::kHttp11;
			}
			else {
				httpVersion_ = HttpVersion::kUnknown;
			}
		}

		//HttpVersion getHttpVersion() const {
		//	return httpVersion_;
		//}

		void setStatus(const HttpResponseStatus & code) {
			statusCode_ = code; 
			switch (code){
			case k200Succeed:
				statusMessage_ = "200 Succeed"; break;
			case k400Failed: 
				statusMessage_ = "400 Failed"; break;
			case k404NotFound:
				statusMessage_ = "404 Not Found"; break;
			default:
				statusMessage_ = "500 UnKnown"; break;
			}
		}

		//HttpResponseStatus getStatusCode() const{
		//	return statusCode_;
		//}

		//const string& getStatusMessage() const{
		//	return statusMessage_; 
		//}

		void addHeader(const string & name, const string & val) { 
			header_.insert(std::make_pair(name, val));
		}

		void setBody(string & body) { body_ = body; }

		void setBody(string && body) { body_ = std::move(body); }

		void setFilePath(string & path) { filePath_ = path; }

		void setFilePath(string && path) { filePath_ = std::move(path); }

		void appendToBuffer(Buffer & buff);

		void appendFileToBuffer(Buffer & buff);

	private:
		HttpVersion httpVersion_;
		HttpResponseStatus statusCode_;
		string statusMessage_;
		std::unordered_map<string, string> header_;
		string body_;
		string filePath_;

		string getStringHttpVersion() const {
			std::string result;
			switch (httpVersion_)
			{
			case HttpVersion::kHttp10:
				result = "HTTP/1.0"; break;
			case HttpVersion::kHttp11:
				result = "HTTP/1.1"; break;
			default:
				result = "Unknown"; break;
			}
			return result;
		}

		string getStringStatusCode() const {
			std::string result;
			switch (statusCode_)
			{
			case ChatRoom::HttpResponse::k200Succeed:
				result = "200";  break;
			case ChatRoom::HttpResponse::k400Failed:
				result = "400";  break;
			case ChatRoom::HttpResponse::k404NotFound:
				result = "404";  break;
			default:
				result = "500";  break;
			}
			return result;
		}
	};
}
#endif // ! ChatRoom_HttpResponse_H
