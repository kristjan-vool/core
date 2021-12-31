#include <core/router/router.hpp>

#include <string>
#include <regex>

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
	// Generate request headers.
	Request request = Request();

	int end = 0;

	// Read request header without content by 1 char until 2 newlines.
	while (end != 4) {
		read(connection , &request.buffer, 1);
		request.bufferUpdated();
		end = (request.buffer == 10 || request.buffer == 13) ? end + 1 : 0;
	}

	std::smatch matches;


	// Find content length.
	if (request.getContentLength()) {
		std::string data;
		// Read content to request header.
		for (int i = 0; i < request.getContentLength(); i++) {
			read(connection , &request.buffer, 1);
			data += request.buffer;
		}

		request.setData(data);
	}

	// Valid request.
	if (request.isValid()) {
		Response response = Response(connection);

		// Check if method is allowed.
		if (this -> routes.find(request.getMethod()) != this -> routes.end()) {
			auto routes = this -> routes.at(request.getMethod());

			// Check if route is allowed.
			// route -> first is route regex.
			// route -> second is route method to use for routing.
			for (auto route = routes.begin(); route != routes.end(); route++) {
				// Regex.
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

				// Not regex.
				} else if (request.getURL() == route -> first) {
					this -> routes.at(request.getMethod()).at(route -> first)(request, response);
				}
			}

			// Route not found, respond with 404.
			if (!response.isSent()) {
				response.status(404).send();
			}

		// Invalid method, respond with 404.
		} else {
			response.status(404).send();
		}

	// Invalid request, close connection.
	} else {
		close(connection);
	}
}
