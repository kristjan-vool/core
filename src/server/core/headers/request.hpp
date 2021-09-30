#ifndef CORE_REQUEST_HPP
#define CORE_REQUEST_HPP

#include <string>
#include <optional>
#include <map>

#include <nlohmann/json.hpp>

class Request {
	public:
		Request(const std::string& request);
		const std::string method;
		const std::string url;
		const std::string version;
		const nlohmann::json data;
		const std::map<std::string, std::string> cookies;

		bool isValid() const;
		const std::string *const getCookie(const std::string& cookie) const;

	private:
		std::string setHead(const std::string& request, const int& position);
		nlohmann::json setData(const std::string& request);
		std::map<std::string, std::string> setCookies(const std::string& request);
};

#endif
