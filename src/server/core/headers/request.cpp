#include <core/headers/request.hpp>
#include <iostream>
#include <ostream>
#include <regex>

/**
 * Get data sent trough body and url.
 */
const nlohmann::json &Request::getData() const {
	return data;
}

/**
 * Get header method.
 */
const std::string &Request::getMethod() const {
	return method;
}

/**
 * Get header URL.
 */
const std::string &Request::getURL() const {
	return url;
}

/**
 * Get header version.
 */
const std::string &Request::getVersion() const {
	return version;
}

/**
 * Set data from header body.
 */
void Request::setData(const std::string &data) {
	try {
		this -> data.update(nlohmann::json::parse(data));
	} catch (...) {}
}

/**
 * Check if received request is valid.
 * @return true if valid.
 */
bool Request::isValid() const {
	return !method.empty() && !url.empty() && !version.empty();
}

/**
 * Get header cookies.
 */
std::map<std::string, std::string> Request::getCookies() const {
	return cookies;
}

/**
 * Get header cookie by cookie name.
 * @param cookie - Cookie name to get.
 */
std::optional<std::string> Request::getCookie(const std::string &cookie) const {
	// Cookie found.
	if (this -> cookies.find(cookie) != this -> cookies.end()) {
		return cookies.at(cookie);

	// Cookie not found.
	} else {
		return std::nullopt;
	}
}

/**
 * Get header content length.
 */
const int &Request::getContentLength() const {
	return content_length;
}

/**
 * Header buffer updated.
 * Set header method, url and data, version.
 */
void Request::bufferUpdated() {
	const int ascii_newline = 10;
	const int ascii_return = 13;
	const int ascii_space = 32;

	if (buffer == ascii_newline || buffer == ascii_return || buffer == ascii_space) {
		if (buffer == ascii_newline) {
			lineReceived();
			buffer_line = "";
		}

		if (state == NONE) {
			state = METHOD_SET;
		} else if (state == METHOD_SET) {
			state = URL_SET;
			setDataFromURL();
		} else if (state == URL_SET) {
			state = VERSION_SET;
		}
	} else {
		buffer_line += buffer;
		if (state == NONE) {
			method += buffer;
		} else if (state == METHOD_SET) {
			url += buffer;
		} else if (state == URL_SET) {
			version += buffer;
		}
	}
}

/**
 * Set header data from url parameters.
 */
void Request::setDataFromURL() {
	const size_t question = url.find('?');

	if (question != std::string::npos) {
		std::string data = url.substr(question + 1);
		size_t symbol_equal, symbol_and, split;

		while((symbol_equal = data.find('=')) != std::string::npos) {
			std::string key = data.substr(0, symbol_equal);
			std::string value = data.substr(symbol_equal + 1);
			split = key.length();

			if ((symbol_and = value.find('&')) != std::string::npos) {
				value = value.substr(0, symbol_and);
				split += 2;
			}
			split += value.length();

			this -> data[key] = value;
			data = data.substr(split);
		}
	}
}

/**
 * Request header line received.
 * Check if line is cookies or content length.
 */
void Request::lineReceived() {
	if (buffer_line.starts_with("Cookie:")) {
		this -> cookies = setCookies(buffer_line.substr(7));
	} else if (buffer_line.starts_with("Content-Length:")) {
		this -> content_length = std::stoi(buffer_line.substr(15));
	}
}

/**
 * Set request cookies from header cookies line.
 * @param cookies - cookies line of text from header.
 */
std::map<std::string, std::string> Request::setCookies(const std::string &cookies) {
	std::map<std::string, std::string> cookies_map;
	std::string key, value;
	bool key_set = false;

	// Extract cookies string from request header.
	for (const char &c : cookies) {
		if (c == '=') {
			key_set = true;
		} else if (c == ';') {
			cookies_map[key] = value;
			key = "";
			value = "";
			key_set = false;
		} else if (!key_set) {
			key += c;
		} else {
			value += c;
		}
	}

	if (!key.empty() && !value.empty()) {
		cookies_map[key] = value;
	}

	return cookies_map;
}
