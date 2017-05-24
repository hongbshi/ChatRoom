#ifndef  ChatRoom_HttpRequest_H
#define  ChatRoom_HttpRequest_H
#include <string>
#include <unordered_map>
namespace ChatRoom
{
	using std::string;
	class HttpRequest {
	public:
		enum HttpMethod {
			kGet,
			kPost,
			kHead,
			kPut,
			kDelete,
			kInvalid
		};

		enum HttpVersion {
			kUnknown,
			kHttp10,
			kHttp11
		};

		void setMethod(const HttpMethod& rhs) { method_ = rhs; }

		void setMethod(const char *start, const char *end) {
			std::string str(start, end);
			if (str == "GET") {
				method_ = kGet;
			}
			else if (str == "POST") {
				method_ = kPost;
			}
			else if (str == "HEAD") {
				method_ = kHead;
			}
			else if (str == "PUT") {
				method_ = kPut;
			}
			else if (str == "DELETE") {
				method_ = kDelete;
			}
			else {
				method_ = kInvalid;
			}
		}

		HttpMethod getMethod() const { return method_; }

		std::string getStringMethod() const {
			std::string result;
			switch (method_)
			{
			case kGet: 
				result = "kGet"; break;
			case kPost:
				result = "kPost"; break;
			case kHead:
				result = "kHead"; break;
			case kPut:
				result = "kPut"; break;
			case kDelete:
				result = "kDelete"; break;
			default:
				result = "kInvalid"; break;
			}
			return result;
		}

		void setUrl(const string& url) {
			url_ = url;
		}

		void setUrl(const char *start, const char *end) {
			url_.assign(start, end);
		}

		const string& getUrl() const {
			return url_;
		}

		void setVersion(const HttpVersion& v) {
			version_ = v;
		}

		void setVersion(const char *start, const char *end) {
			std::string str(start, end);
			if (str == "HTTP/1.0") {
				version_ = kHttp10;
			}
			else if (str == "HTTP/1.1") {
				version_ = kHttp11;
			}
			else {
				version_ = kUnknown;
			}
		}

		HttpVersion getVersion() {
			return version_;
		}

		std::string getStringVersion() {
			std::string result;
			switch (version_)
			{
			case ChatRoom::HttpRequest::kUnknown:
				result = "Unknown"; break;
			case ChatRoom::HttpRequest::kHttp10:
				result = "HTTP/1.0"; break;
			case ChatRoom::HttpRequest::kHttp11:
				result = "HTTP/1.1"; break;
			default:
				break;
			}
			return result;
		}

		void addHeader(const string& name, const string& val) {
			headers_.insert(std::make_pair(name, val));
		}

		void addHeader(const char *start, const char *end) {
			const char *colon = start;
			while (colon < end && *colon != ':') ++colon;
			if (colon >= end) return; // not exit colon
			std::string name(start, colon);
			++colon;
			while (colon < end && *colon == ' ') ++colon;
			std::string val(colon, end);
			headers_.insert(std::make_pair(name, val));
		}

		const std::unordered_map<string, string>& getHeader() const {
			return headers_;
		}

		void setBody(const string& body) {
			body_ = body;
		}

		void setBody(const char *start, const char *end) {
			body_.assign(start, end);
		}

		const string& getBody() const {
			return body_;
		}

		void swap(HttpRequest& rhs) {
			std::swap(method_, rhs.method_);
			url_.swap(rhs.url_);
			std::swap(version_, rhs.version_);
			headers_.swap(rhs.headers_);
			body_.swap(rhs.body_);
		}

	private:
		HttpMethod method_;
		string url_;
		HttpVersion version_;
		std::unordered_map<string, string> headers_;
		string body_;
	};
}
#endif // ! ChatRoom_HttpRequest_H
