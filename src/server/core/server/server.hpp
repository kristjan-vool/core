#ifndef CORE_SERVER_HPP
#define CORE_SERVER_HPP

#include <core/router/router.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstddef>
#include <string>


class CoreServer {
	public:
		CoreServer(CoreRouter *const router, const unsigned int port, const unsigned int connections);

		// Routes.
		void get(const std::string &url, void (*route)(const Request *const, Response *const));
		void get(const std::string &url, const std::string &content);
		void post(const std::string &url, void (*route)(const Request *const, Response *const));
		void post(const std::string &url, const std::string &content);

		int start();

	private:
		CoreRouter *const router;

		const unsigned int port;
		const unsigned int connections;

		const int family   = AF_INET;
		const int addr     = INADDR_ANY;
		const int type     = SOCK_STREAM;
		const int protocol = IPPROTO_IP;

		int          server;
		sockaddr_in  address;
		unsigned int address_size;

		void createSocket      (int &server);
		void configureSocket   (int &server);
		void startListener     (int &server);
		void bindSocketAddress (int &server, sockaddr_in &server_address);
		void createAddress     (sockaddr_in &address, unsigned int &address_size);
};

#endif
