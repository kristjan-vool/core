#include <core/server/server.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <netinet/tcp.h>
#include <cstring>
#include <iostream>
#include <ostream>
#include <unistd.h>
#include <thread>

/**
 * Static respond function for router to respond to connection in new thread.
 * @param router     Router to use.
 * @param connection Connection to respond to.
 */
void respond(CoreRouter *const router, int connection) {
	router -> respond(connection);
	close(connection);
}

/**
 * Create new server and start listening for connections.
 * @param port        Port to listen on.
 * @param connections Number of parallel allowed connections.
 */
CoreServer::CoreServer(CoreRouter *const router, const unsigned int port, const unsigned int connections):
	router(router), port(port), connections(connections) {
		this -> createSocket(this -> server);
		this -> configureSocket(this -> server);
		this -> createAddress(this -> address, this -> address_size);
		this -> bindSocketAddress(this -> server, this -> address);
		this -> startListener(this -> server);
}

/**
 * Server GET method startpoint.
 * @param url   Request url.
 * @param route Route method.
 */
void CoreServer::get(const std::string &url, void (*route)(const Request *const, Response *const)) {
	router -> route("GET", url, route);
}

/**
 * Server POST method startpoint.
 * @param url   Request url.
 * @param route Route method.
 */
void CoreServer::post(const std::string &url, void (*route)(const Request *const, Response *const)) {
	router -> route("POST", url, route);
}

/**
 * Create server socket.
 * @param server Server socket.
 */
void CoreServer::createSocket(int &server) {
	server = socket(
		this -> family,
		this -> type,
		this -> protocol
	);

	if (server == -1) {
		perror("Server creation failed: ");
		exit(EXIT_FAILURE);
	}
}

/**
 * Configure server socket.
 * @param server Server socket.
 */
void CoreServer::configureSocket(int &server) {
	int conf = 1;

	if (
		setsockopt(server, SOL_TCP,    TCP_NODELAY,  &conf, sizeof(conf)) == -1 ||
		setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &conf, sizeof(conf)) == -1 ||
		setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, &conf, sizeof(conf)) == -1
	) {
		perror("Unable to configure server: ");
		exit(EXIT_FAILURE);
	}
}

/**
 * Configure address.
 * @param address Server address.
 */
void CoreServer::createAddress(sockaddr_in &address, unsigned int &address_size) {
	address_size = sizeof(address);
	memset(&address, 0, address_size);
	address.sin_family      = this -> family;
	address.sin_addr.s_addr = this -> addr;
	address.sin_port        = htons(this -> port);
}

/**
 * Bind address to server.
 * @param server  Server socket.
 * @param address Server address.
 */
void CoreServer::bindSocketAddress(int &server, sockaddr_in &address) {
	if (bind(server, (struct sockaddr*) &address, sizeof(address)) == -1) {
		perror("Server address already binded: ");
		exit(EXIT_FAILURE);
	}
}

/**
 * Start server connections listener.
 * @param server socket
 */
void CoreServer::startListener(int &server) {
	if (listen(server, this -> connections) == -1) {
		perror("Unable to listen on port: ");
		exit(EXIT_FAILURE);
	}
}

/**
 * Start server responder.
 */
int CoreServer::start() {
	std::cout << "Server running on port: " << this -> port << std::endl;

	int &server = this -> server;
	sockaddr_in &address = this -> address;

	// Server main loop.
	while(true) {
		// Check for connection.
		int connection = (accept(server, (struct sockaddr*) &address, (socklen_t*) &address_size));

		// Connection found.
		if (connection != -1) {
			std::thread thread(respond, router, connection);
			thread.detach();

			// respond(router, connection);

		// Connection not found.
		} else {
			perror("Request failed: ");
			exit(EXIT_FAILURE);
			close(connection);
		}
	}

	std::cout << "Server closed." << std::endl;
	return EXIT_FAILURE;
}
