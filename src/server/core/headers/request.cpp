#include <core/headers/request.hpp>
#include <iostream>
#include <ostream>
#include <regex>

/**
 * HTTP Request Headers.
 * @param request Request headers text.
 */
Request::Request(const std::string &request):
	method (this -> setHead(request, 1)),
	url    (this -> setURL(this -> setHead(request, 2))),
	version(this -> setHead(request, 3)),
	data(this -> setData(request)) {
}

nlohmann::json Request::setData(const std::string& request) {
	std::regex regex("(.+)$");
	std::smatch matches;

	if (std::regex_search(request, matches, regex)) {
		return nlohmann::json::parse(matches[0].str());
	}

	return {};
}

std::string Request::setURL(const std::string &url) {
	return url.compare("/") == 0 ? "/index" : url;
}

/**
 * Check if received request is valid.
 * @return true if valid.
 */
const bool Request::isValid() const {
	return this -> method.compare("") != 0;
}

/**
 * Set request headers head (method, url, version).
 * @param  request Request headers text.
 * @param  pos     Position of head to extract.
 * @return         Head part from given position.
 */
std::string Request::setHead(const std::string &request, const long unsigned int pos) {
	std::regex r("^(.*) (.*) (.*)\r\n");
	std::smatch match;
	std::regex_search(request.begin(), request.end(), match, r);

	if (match.size() >= pos + 1) {
		return match[pos];
	} else {
		return "";
	}
}
