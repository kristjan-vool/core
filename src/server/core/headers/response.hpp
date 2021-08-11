#ifndef CORE_RESPONSE_HPP
#define CORE_RESPONSE_HPP

#include <string>

class Response {
	public:
		Response(const int &connection);

		std::string version      = "HTTP/1.1";
		unsigned int status_code = 200;
		std::string content_type = "text/html";
		std::string charset      = "utf-8";
		std::string content      = "";

		Response *const type(const std::string &content_type);
		Response *const status(const int &status_code);

		const bool isSent() const;
		const bool isRedirected() const;

		void json(const std::string &content);
		void send();
		void send(const std::string &content);
		void redirect(const std::string &url);

	private:
		const int &connection;
		bool sent = false;

		const bool throwIsSent() const;
		std::string getHead();
		std::string getContentType();
		std::string getCharset();
		std::string getContentLength();
		std::string getRedirect();
		std::string getContent();
};

#endif
