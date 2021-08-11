#ifndef CORE_REQUEST_HPP
#define CORE_REQUEST_HPP

#include <string>
#include <optional>

class Request {
	public:
		Request(const std::string &request);
		const std::string method;
		const std::string url;
		const std::string version;
		const bool isValid() const;

	private:
		std::string setHead(const std::string &request, const long unsigned int pos);
		std::string setURL(const std::string &url);
};

#endif
