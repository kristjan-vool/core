#include <core/headers/request.hpp>
#include <iostream>
#include <ostream>
#include <regex>

static std::regex regex_cookies("Cookie: (.*)");
static std::regex regex_cookie("(.*)=(.*)");
static std::regex regex_head("^(.*) (.*) (.*)\r\n");
static std::regex regex_data("(.+)$");

/**
 * HTTP Request Headers.
 * @param request Request headers text.
 */
Request::Request(const std::string& request):
	method (setHead(request, 1)),
	url    (setHead(request, 2)),
	version(setHead(request, 3)),
	data   (setData(request)),
	cookies(setCookies(request)) {
}

std::map<std::string, std::string> Request::setCookies(const std::string& request) {
	std::map<std::string, std::string> cookies_map;
	std::smatch matches;

	// Extract cookies string from request header.
	if (std::regex_search(request, matches, regex_cookies)) {
		std::string cookies = matches[1].str() + "; ";
		size_t pos = 0;

		// Split cookies by ;
		while ((pos = cookies.find("; ")) != std::string::npos) {
			std::string cookie = cookies.substr(0, pos);

			// Find cookie key and value with regex.
			if (std::regex_search(cookie, matches, regex_cookie)) {
				cookies_map[matches[1].str()] = matches[2].str();
			}
			cookies.erase(0, pos + 2);
		}
	}

	return cookies_map;
}

const std::string *const Request::getCookie(const std::string& cookie) const {
	// Cookie found.
	if (this -> cookies.find(cookie) != this -> cookies.end()) {
		return &cookies.at(cookie);

	// Cookie not found.
	} else {
		return nullptr;
	}
}

nlohmann::json Request::setData(const std::string& request) {
	std::smatch matches;

	try {
		if (std::regex_search(request, matches, regex_data)) {
			return nlohmann::json::parse(matches[0].str());
		}
	} catch (...) {}

	return {};
}

/**
 * Check if received request is valid.
 * @return true if valid.
 */
bool Request::isValid() const {
	return this -> method.compare("") != 0;
}

/**
 * Set request headers head (method, url, version).
 * @param  request Request headers text.
 * @param  pos     Position of head to extract.
 * @return         Head part from given position.
 */
std::string Request::setHead(const std::string& request, const int& position) {
	std::smatch matches;
	std::regex_search(request, matches, regex_head);

	return matches[position];
}
