#ifndef CORE_ROUTER_HPP
#define CORE_ROUTER_HPP

#include <core/headers/request.hpp>
#include <core/headers/response.hpp>

#include <string>
#include <map>

class CoreRouter {
	public:
		void respond(const int &connection);
	protected:
	private:
		void route(const std::string &method, const std::string &url, void (*route)(const Request &request, Response &response));
		std::map<std::string, std::map<std::string, void (*)(const Request&, Response&)>> routes;

	friend class CoreServer;
};

#endif
