#include <core/headers/request.hpp>
#include <iostream>
#include <ostream>
#include <boost/algorithm/string.hpp>

Request::Request(std::string &headers):
	method (this -> readSegment  (headers)),
	url    (this -> readSegment  (headers)),
	path   (this -> readPathQuery(this -> url, true)),
	query  (this -> readPathQuery(this -> url, false)),
	version(this -> readSegment  (headers)),
	cookies(this -> readCookies  (headers)),
	data   (this -> readData     (headers, this -> query))
{
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
 * Get header path.
 */
const std::string &Request::getPath() const {
	return path;
}

/**
 * Get header query.
 */
const std::string &Request::getQuery() const {
	return query;
}

/**
 * Get header version.
 */
const std::string &Request::getVersion() const {
	return version;
}

/**
 * Read segment where seperator is space or newline.
 * @param  headers - Fully read request headers that will be modified after this method is called.
 * @return segment and modify the source headers string.
 */
std::string Request::readSegment(std::string &headers) const {
	std::string segment;

	for (auto iterator = headers.begin(); iterator != headers.end(); iterator++) {
		if (*iterator == ' ' || *iterator == '\n' || *iterator == '\r') break;
		segment += *iterator;
	}

	headers = headers.substr(segment.length() + 1);
	return segment;
}

/**
 * Read path or query from url without modifying any sources.
 * @param  url  - URL read from headers.
 * @param  path - True if reading path, false if reading query.
 * @return path or query from URL.
 */
std::string Request::readPathQuery(const std::string &url, const bool &path) const {
	std::size_t query_start = url.find('?');

	// Query found.
	if (query_start != std::string::npos) {
		if (path) {
			return url.substr(0, query_start);
		} else {
			return url.substr(query_start);
		}

	// Query not found.
	} else return path ? url : "";
}

/**
 * Read headers data, body data and query data.
 * @param headers - Fully read request headers that will be modified after this method is called.
 * @param query - Data query that is part of the URL.
 * @return map of any data.
 */
std::map<std::string, std::any> Request::readData(std::string &headers, const std::string &query) const {
	std::map<std::string, std::any> data;

	// Query data.
	if (!query.empty()) {
		data = this -> readPairs(query.substr(1), "=", "&");
	}

	return data;
}

/**
 * Get reference to all data read from headers, body and url query.
 */
const std::map<std::string, std::any> &Request::getData() const {
	return this -> data;
}

/**
 * Get copy of one specific data read from headers, body or url query. Value can be empty.
 * @param  key - Data key to look for.
 * @return copy from data that can be empty.
 */
std::any Request::getData(const std::string &key) const {
	std::any data;

	// Get data from specific location.
	if (this -> data.contains(key)) {
		data = this -> data.at(key);
	}

	return data;
}

/**
 * Get reference to all cookies.
 */
const std::map<std::string, std::any> &Request::getCookies() const {
	return this -> cookies;
}

/**
 * Get copy of one specific cookie. Value can be empty.
 * @param  key - Cookie name to look for.
 * @return copy from cookies that can be empty.
 */
std::any Request::getCookie(const std::string &key) const {
	std::any cookie;

	// Get data from specific location.
	if (this -> cookies.contains(key)) {
		cookie = this -> cookies.at(key);
	}

	return cookie;
}

/**
 * Read cookies from request headers.
 * @param headers - Fully read request headers that will be modified after this method is called.
 * @return map of any values.
 */
std::map<std::string, std::any> Request::readCookies(std::string &headers) const {
	std::map<std::string, std::any> cookies;

	std::string headers_copy = boost::algorithm::to_lower_copy(headers);
	size_t cookies_start = headers_copy.find("cookie: ");

	if (cookies_start != std::string::npos) {
		size_t cookies_end = headers_copy.find('\n', cookies_start);
		cookies = this -> readPairs(headers.substr(cookies_start + 8, cookies_end), "=", ";");

		// Remove cookies from headers.
		headers = headers.substr(0, cookies_start) + headers.substr(cookies_end + 1);
	}

	return cookies;
}

/**
 * General method to read pairs of data (for example query, cookies).
 * @param source    - Source string where pairs are located.
 * @param equal     - Equal symbol that splits key and value.
 * @param separator - Separator symbol that splits key and value pairs.
 * @return map of any values.
 */
std::map<std::string, std::any> Request::readPairs(std::string source, const std::string &equal, const std::string &separator) const {
	std::map<std::string, std::any> pairs;

	// Loop source until all pairs have been used.
	size_t symbol_equal, symbol_separator, split;
	while ((symbol_equal = source.find(equal)) != std::string::npos) {
		std::string key = source.substr(0, symbol_equal);
		std::string value = source.substr(symbol_equal + 1);
		split = key.length();

		// Remove other keys/values from current value.
		if ((symbol_separator = value.find(separator)) != std::string::npos) {
			value = value.substr(0, symbol_separator);
			split += 1;
		}
		split += value.length();

		// Boolean.
		if (value == "false" || value == "true") {
			pairs[key] = value == "true";
		} else {

			// Integer.
			try {
				pairs[key] = std::stoi(value);

			// String.
			} catch (...) {
				pairs[key] = value;
			}
		}
		// Cut found pair from source.
		source = source.substr(split);
	}

	return pairs;
}
