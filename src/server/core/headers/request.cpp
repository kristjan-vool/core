#include <core/headers/request.hpp>
#include <iostream>
#include <ostream>
#include <regex>

void Request::bufferUpdated() {
    if (buffer == 10 || buffer == 13 || buffer == 32) {
        if (buffer == 10) {
            lineReceived();
            buffer_line = "";
        }

        if (state == NONE) {
            state = METHOD_SET;
        } else if (state == METHOD_SET) {
            state = URL_SET;
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

void Request::lineReceived() {
    if (buffer_line.starts_with("Cookie:")) {
        this -> cookies = setCookies(buffer_line.substr(7));
    } else if (buffer_line.starts_with("Content-Length:")) {
        this -> content_length = std::stoi(buffer_line);
    }
}

int Request::getContentLength() {
    return content_length;
}

std::map<std::string, std::string> Request::setCookies(const std::string& cookies) {
	std::map<std::string, std::string> cookies_map;

    std::string key;
    std::string value;
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

std::optional<std::string> Request::getCookie(const std::string& cookie) const {
	// Cookie found.
	if (this -> cookies.find(cookie) != this -> cookies.end()) {
		return cookies.at(cookie);

	// Cookie not found.
	} else {
		return std::nullopt;
	}
}

const nlohmann::json& Request::getData() const {
    return data;
}

const std::string& Request::getMethod() const {
    return method;
}

const std::string& Request::getURL() const {
    return url;
}

const std::string& Request::getVersion() const {
    return version;
}

void Request::setData(const std::string& data) {
	std::smatch matches;

	try {
        this -> data = nlohmann::json::parse(matches[0].str());
	} catch (...) {}
}

/**
 * Check if received request is valid.
 * @return true if valid.
 */
bool Request::isValid() const {
	return !method.empty() && !url.empty() && !version.empty();
}
