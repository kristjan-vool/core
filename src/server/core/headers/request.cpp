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

nlohmann::json Request::setCookies(const std::string& request) {
	nlohmann::json cookies_json;
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
				std::string key = matches[1].str();
				std::string value = matches[2].str();

				// Boolean cookie.
				if (value == "true" || value == "false") {
					cookies_json[key] = (value == "true");
				} else {
					// Number cookie.
					try {
						cookies_json[key] = std::stoi(value);

					// String cookie.
					} catch (...) {
						cookies_json[key] = value;
					}
				}
			}
			cookies.erase(0, pos + 2);
		}
	}

	return cookies_json;
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
const bool Request::isValid() const {
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
