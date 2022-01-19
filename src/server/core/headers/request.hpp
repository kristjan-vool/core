#ifndef CORE_REQUEST_HPP
#define CORE_REQUEST_HPP

#include <string>
#include <map>
#include <any>
#include <nlohmann/json.hpp>

class Request {
	public:
		Request(std::string &headers);
		const std::string &getMethod()  const;
		const std::string &getURL()     const;
		const std::string &getPath()    const;
		const std::string &getQuery()   const;
		const std::string &getVersion() const;

		// Data.
		const std::map<std::string, std::any> &getData() const;
		std::any getData(const std::string &key) const;

		// Cookies.
		const std::map<std::string, std::any> &getCookies() const;
		std::any getCookie(const std::string &key) const;

	private:
		const std::string method;
		const std::string url;
		const std::string path;
		const std::string query;
		const std::string version;
		std::map<std::string, std::any> data;
		std::map<std::string, std::any> cookies;

		std::string readSegment(std::string &headers) const;
		std::string readPathQuery(const std::string &url, const bool &path) const;
		std::map<std::string, std::any> readData(std::string &headers, const std::string &query) const;
		std::map<std::string, std::any> readCookies(std::string &request) const;
		std::map<std::string, std::any> readPairs(std::string source, const std::string &equal, const std::string &seperator) const;
};

#endif
