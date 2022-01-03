#ifndef CORE_REQUEST_HPP
#define CORE_REQUEST_HPP

#include <string>
#include <optional>
#include <map>

#include <nlohmann/json.hpp>

enum RequestState {
	NONE,
	METHOD_SET,
	URL_SET,
	VERSION_SET
};

class Request {
	public:
		Request() {};
		char buffer;
		void bufferUpdated();
		void lineReceived();
		void setData(const std::string &data);

		const std::string &getMethod() const;
		const std::string &getURL() const;
		const std::string &getQuery() const;
		const std::string &getPath() const;
		const std::string &getVersion() const;
		const int &getContentLength() const;

		bool isValid() const;
		std::optional<std::string> getCookie(const std::string &cookie) const;
		std::map<std::string, std::string> getCookies() const;
		const nlohmann::json &getData() const;

	private:
		std::string buffer_line;
		std::string method;
		std::string url;
		std::string query;
		std::string path;
		std::string version;
		int content_length = 0;
		nlohmann::json data;
		std::map<std::string, std::string> cookies;
		std::map<std::string, std::string> setCookies(const std::string &request);
		RequestState state = NONE;
		void setDataFromURL();
};

#endif
