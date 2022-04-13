#ifndef CORE_REQUEST_HPP
#define CORE_REQUEST_HPP

#include <string>
#include <map>
#include <any>

class Request {
	public:
		Request(std::string &headers);
		const std::string &getHeaders() const;
		const std::string &getMethod() const;
		const std::string &getURL() const;
		const std::string &getPath() const;
		const std::string &getQuery() const;
		const std::string &getVersion() const;
		const std::string &getContentType() const;
		const std::string &getBody() const;

		// Data.
		const std::map<std::string, std::any> &getData() const;
		std::any getData(const std::string &key) const;

		// Cookies.
		const std::map<std::string, std::any>& getCookies() const;
		std::any getCookie(const std::string &key) const;

		bool isValid() const;

	private:
		const std::string headers;
		const std::string method;
		const std::string url;
		const std::string path;
		const std::string query;
		const std::string version;
		const std::string content_type;
		const std::string body;
		std::map<std::string, std::any> data;
		std::map<std::string, std::any> cookies;

		std::string readBody(std::string &headers) const;
		std::string readLineValue(std::string &headers, std::string key) const;
		std::string readSegment(std::string &headers) const;
		std::string readPathQuery(const std::string &url, const bool &path) const;
		std::map<std::string, std::any> readData(const std::string &query, const std::string &body, const std::string &content_type) const;
		std::map<std::string, std::any> readPairs(std::string source, const std::string &equal, const std::string &seperator) const;
};

#endif
