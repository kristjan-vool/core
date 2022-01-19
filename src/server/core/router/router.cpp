#include <core/router/router.hpp>

#include <string>
#include <regex>
#include <iostream>
#include <sys/socket.h>

/**
 * Add route to the list of routes.
 * @param url   of the route.
 * @param route method that responds to the connection.
 */
void CoreRouter::route(const std::string &method, const std::string &url, void (*route)(const Request&, Response&)) {
	this -> routes[method][url] = route;
}

/**
 * Respond to the request connection.
 * @param connection Client request.
 */
void CoreRouter::respond(const int &connection) {
	std::string headers;
	char buffer = 0;

	// Read request headers.
	while (recv(connection, &buffer, 1, (buffer == 0 ? 0 : MSG_DONTWAIT)) > 0) {
		headers += buffer;
	}

	// Generate Request and Response.
	try {
		Request request = Request(headers);
		Response response = Response(connection);

		// Check if method is allowed.
		if (this -> routes.find(request.getMethod()) != this -> routes.end()) {
			auto routes = this -> routes.at(request.getMethod());

			// Check if route is allowed.
			// route -> first is route regex.
			// route -> second is route method to use for routing.
			for (auto route = routes.begin(); route != routes.end(); route++) {
				// Route is defined with regex.
				if (route -> first.find("*") != std::string::npos) {
					std::regex r(route -> first);
					std::smatch match;
					std::regex_search(request.getURL(), match, r);

					// Route regex matches request url.
					if (match.size() > 0) {
						this -> routes.at(request.getMethod()).at(route -> first)(request, response);

						// Route found, break routes looping.
						break;
					}

				// Route is specific.
				} else if (request.getURL() == route -> first) {
					this -> routes.at(request.getMethod()).at(route -> first)(request, response);
					break;
				}
			}

			// Invalid route or invalid route handler. Respond with 404.
			if (!response.isSent()) {
				response.status(404).send();
			}

		// Invalid method. Respond with 404.
		} else {
			response.status(404).send();
		}

	// Invalid request headers.
	} catch (...) {
		close(connection);
	}
}
