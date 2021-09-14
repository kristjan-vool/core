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
 * Send response to the request as JSON.
 * @param content JSON string.
 */
void Response::json(const std::string& content) {
	this -> throwIsSent();
	this -> content_type = "application/json";
	this -> content = content;
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
std::string Response::getHead() {
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
std::string Response::getRedirect() {
	std::string redirect = "";

	if (this -> isRedirected()) {
		redirect = "location: " + this -> content + "\r\n";
	}

	return redirect;
}

/**
 * Get response headers content type and charset.
 * @return headers content type and charset.
 */
std::string Response::getContentType() {
	std::string content_type = "";

	if (!this -> isRedirected()) {
		content_type = "content-type: " + this -> content_type + "; " + this -> getCharset() + "\r\n";
	}

	return content_type;
}

/**
 * Get response content.
 * @return headers content.
 */
std::string Response::getContent() {
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
std::string Response::getCharset() {
	return "charset=" + this -> charset;
}

/**
 * Get response headers content length.
 * @return headers content length.
 */
std::string Response::getContentLength() {
	std::string content_length = "";

	if (!this -> isRedirected()) {
		content_length = "content-length: " + std::to_string(this -> content.length()) + "\r\n";
	}

	return content_length;
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
