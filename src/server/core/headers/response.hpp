#ifndef CORE_RESPONSE_HPP
#define CORE_RESPONSE_HPP

#include <string>

#include <map>

class Response {
	public:
		Response(const int& connection);

		std::string version      = "HTTP/1.1";
		unsigned int status_code = 200;
		std::string content_type = "text/html";
		std::string charset      = "utf-8";
		std::string content      = "";

		Response& type(const std::string& content_type);
		Response& status(const unsigned int& status_code);

		bool isSent() const;
		bool isRedirected() const;

		void sendJSON(const std::string& json);
		void send();
		void send(const std::string& content);
		void redirect(const std::string& url);

		void setCookie(const std::string &key, const std::string& value);
		void setCookie(const std::string &key, const std::string& value, const std::string& path, const int& age);

	private:
		const int& connection;
		bool sent = false;
		std::map<std::string, std::map<std::string, std::string>> cookies;

		bool throwIsSent() const;
		std::string getHead() const;
		std::string getContentType() const;
		std::string getCharset() const;
		std::string getContentLength() const;
		std::string getRedirect() const;
		std::string getContent() const;
		std::string getCookies() const;
};

#endif
