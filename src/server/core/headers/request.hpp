#ifndef CORE_REQUEST_HPP
#define CORE_REQUEST_HPP

#include <string>
#include <optional>

#include <nlohmann/json.hpp>

class Request {
	public:
		Request(const std::string &request);
		const std::string method;
		const std::string url;
		const std::string version;
		const nlohmann::json data;
		const bool isValid() const;

	private:
		std::string setHead(const std::string &request, const int& position);
		std::string setURL(const std::string &url);
		nlohmann::json setData(const std::string& request);
};

#endif
