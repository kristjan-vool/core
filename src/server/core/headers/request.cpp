#include <core/headers/request.hpp>
#include <iostream>
#include <ostream>
#include <regex>

/**
 * HTTP Request Headers.
 * @param request Request headers text.
 */
Request::Request(const std::string& request):
	method (this -> setHead(request, 1)),
	url    (this -> setURL(this -> setHead(request, 2))),
	version(this -> setHead(request, 3)),
	data(this -> setData(request)),
	cookies(this -> setCookies(request)) {
}

nlohmann::json Request::setCookies(const std::string& request) {
	nlohmann::json cookies_json;
	std::regex regex("Cookie: (.*)");
	std::smatch matches;

	// Extract cookies string from request header.
	if (std::regex_search(request, matches, regex)) {
		std::string cookies = matches[1].str() + "; ";
		size_t pos = 0;

		// Split cookies by ;
		while ((pos = cookies.find("; ")) != std::string::npos) {
			std::string cookie = cookies.substr(0, pos);
			std::regex regex_cookie("(.*)=(.*)");

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
	std::regex regex("(.+)$");
	std::smatch matches;

	if (std::regex_search(request, matches, regex)) {
		return nlohmann::json::parse(matches[0].str());
	}

	return {};
}

std::string Request::setURL(const std::string& url) {
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
std::string Request::setHead(const std::string& request, const int& position) {
	std::regex regex("^(.*) (.*) (.*)\r\n");
	std::smatch matches;
	std::regex_search(request, matches, regex);

	return matches[position];
}
