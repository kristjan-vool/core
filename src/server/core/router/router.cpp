#include <core/router/router.hpp>

#include <string>
#include <unistd.h>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <regex>

/**
 * Add route to the list of routes.
 * @param url   of the route.
 * @param route method that responds to the connection.
 */
void CoreRouter::route(const std::string &method, const std::string &url, void (*route)(const Request *const, Response *const)) {
	this -> routes[method][url] = route;
}

/**
 * Respond to the request connection.
 * @param connection Client request.
 */
void CoreRouter::respond(const int &connection) {
	char buffer; int end = 0;
	std::string request_header = "";

	// Read request header by 1 char until 2 newlines.
	while (end != 4 && read(connection , &buffer, 1)) {
		end = (buffer == 10 || buffer == 13) ? end + 1 : 0;
		request_header += buffer;
	}

	// Generate request headers.
	const Request *const request = new Request(request_header);

	// Valid request.
	if (request -> isValid()) {
		Response *const response = new Response(connection);

		// Check if method is allowed.
		if (this -> routes.find(request -> method) != this -> routes.end()) {
			auto routes = this -> routes.at(request -> method);

			// Check if route is allowed.
			// route -> first is route regex.
			// route -> second is route method to use for routing.
			for (auto route = routes.begin(); route != routes.end(); route++) {
				std::regex r(route -> first);
				std::smatch match;
				std::regex_search(request -> url.begin(), request -> url.end(), match, r);

				// Route regex matches request url.
				if (match.size() > 0) {
					this -> routes.at(request -> method).at(route -> first)(request, response);

					// Route found, break routes looping.
					break;
				}
			}

			// Route not found, respond with 404.
			if (!response -> isSent()) {
				response -> status(404) -> send();
			}

		// Invalid request, respond with 404.
		} else {
			response -> status(404) -> send();
		}

		delete(response);
	}

	delete(request);
}
