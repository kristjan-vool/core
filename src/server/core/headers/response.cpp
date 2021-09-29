#include <core/headers/response.hpp>
#include <core/status/status.hpp>

#include <iostream>
#include <ostream>
#include <unistd.h>
#include <stdexcept>

/**
 * HTTP Response Headers.
 * @param connection Request connection where to respond.
 */
Response::Response(const int& connection):
	connection(connection) {
}

/**
 * Set cookie from boolean, int, string or null value.
 * @param key   - Cookie key.
 * @param value - Cookie value.
 */
void Response::setCookie(const std::string& key, const std::string& value) {
	std::map<std::string, std::string> cookie;
	cookie["cookie"] = value;

	this -> cookies[key] = cookie;
}

/**
 * Set cookie from boolean, int, string or null value with Max-Age.
 * @param key   - Cookie key.
 * @param value - Cookie value.
 */
void Response::setCookie(const std::string& key, const std::string& value, const std::string& path, const int& age) {
	std::map<std::string, std::string> cookie;
	cookie["cookie"] = value;
	cookie["path"] = path;
	cookie["age"] = std::to_string(age);

	this -> cookies[key] = cookie;
}

/**
 * Send response to the request as JSON.
 * @param content JSON string.
 */
void Response::sendJSON(const std::string& json) {
	this -> throwIsSent();
	this -> content_type = "application/json";
	this -> content = json;
	this -> send();
}

/**
 * Set response headers content-type.
 * @param  content_type Content-Type to set for headers.
 * @return              self.
 */
Response& Response::type(const std::string& content_type) {
	this -> throwIsSent();
	this -> content_type = content_type;
	return *this;
}

/**
 * Set response headers status.
 * @param  status_code HTTP status code.
 * @return             self.
 */
Response& Response::status(const unsigned int& status_code) {
	this -> throwIsSent();
	this -> status_code = status_code;
	return *this;
}

/**
 * Default response redirect.
 * @param url to redirect to.
 */
void Response::redirect(const std::string& url) {
	this -> throwIsSent();
	this -> status_code = 302;
	this -> content = url;

	this -> send();
}

/**
 * Get response headers head text.
 * @return headers head (version, code, message).
 */
std::string Response::getHead() const {
	std::string head = "";

	head.append(this -> version + " ");
	head.append(std::to_string(this -> status_code) + " ");
	head.append(Status::get(this -> status_code) + "\r\n");

	return head;
}

/**
 * Get response headers redirect location.
 * @return headers location if redirected.
 */
std::string Response::getRedirect() const {
	std::string redirect = "";

	if (this -> isRedirected()) {
		redirect = "Location: " + this -> content + "\r\n";
	}

	return redirect;
}

/**
 * Get response headers content type and charset.
 * @return headers content type and charset.
 */
std::string Response::getContentType() const {
	std::string content_type = "";

	if (!this -> isRedirected()) {
		content_type = "Content-Type: " + this -> content_type + "; " + this -> getCharset() + "\r\n";
	}

	return content_type;
}

/**
 * Get response content.
 * @return headers content.
 */
std::string Response::getContent() const {
	std::string content = "";

	if (!this -> isRedirected()) {
		content.append("\r\n");
		content.append(this -> content);
	}

	return content;
}

/**
 * Get response headers charset.
 * @return headers charset.
 */
std::string Response::getCharset() const {
	return "charset=" + this -> charset;
}

/**
 * Get response headers content length.
 * @return headers content length.
 */
std::string Response::getContentLength() const {
	std::string content_length = "";

	if (!this -> isRedirected()) {
		content_length = "Content-Length: " + std::to_string(this -> content.length()) + "\r\n";
	}

	return content_length;
}

/**
 * Get response headers cookies to be set.
 * @return headers cookies.
 */
std::string Response::getCookies() const {
	std::string cookies = "";

	for (const auto& [key, cookie] : this -> cookies) {
		cookies.append("Set-Cookie: " + key + "=" + cookie.at("cookie"));
		cookies.append(cookie.contains("age") ? "; Max-Age=" + cookie.at("age") : "");
		cookies.append(cookie.contains("path") ? "; Path=" + cookie.at("path") : "");
		cookies.append("; Secure; HttpOnly\r\n");
	}

	std::cout << cookies << std::endl;

	return cookies;
}

/**
 * Send response to the request.
 */
void Response::send() {
	this -> throwIsSent();

	// Generate headers.
	std::string response = "";
	response.append(this -> getHead());
	response.append(this -> getRedirect());
	response.append(this -> getCookies());
	response.append(this -> getContentType());
	response.append(this -> getContentLength());
	response.append(this -> getContent());

	// Respond to request.
	write(this -> connection , response.c_str() , response.length());
	close(this -> connection);

	// Set headers sent.
	this -> sent = true;
}

/**
 * Send response to the request.
 * @param content Content to send to the request.
 */
void Response::send(const std::string& content) {
	this -> content = content;
	this -> send();
}

/**
 * Check wether response headers are already sent.
 * @return true and throw exception when sent.
 */
bool Response::throwIsSent() const {
	if (this -> sent) {
		throw std::runtime_error("Error: Response headers already sent.");
	}

	return this -> sent;
}

/**
 * Check wether response headers are already sent.
 * @return true and throw exception when sent.
 */
bool Response::isSent() const {
	return this -> sent;
}

/**
 * Check wether response is redirected.
 * @return true if redirected.
 */
bool Response::isRedirected() const {
	return this -> status_code >= 300 && this -> status_code < 400;
}
