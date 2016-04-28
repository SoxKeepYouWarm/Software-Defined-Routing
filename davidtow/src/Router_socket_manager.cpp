#include <iostream>

#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>

#include "router.h"
#include "Router_socket_manager.h"


Router_socket_manager::Router_socket_manager(Router* router, const char* port) {
	this->router = router;
	this->port = new char[6];
	strcpy(this->port, port);
	this->listener = 0;
	this->addrlen = 0;
	this->res = 0;
	this->p = 0;
	this->MAXIMUM_CONNECTIONS = 1;
	std::cout << "ROUTER_SOCKET_MANAGER: initialized with port: "
			<< port << std::endl;
}


void Router_socket_manager::initialize_addrinfo() {

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	int rv;
	if ((rv = getaddrinfo(NULL, port, &hints, &res)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}

}


void Router_socket_manager::handle_connection(int fd) {

	std::cout << "ROUTER_SOCKET_MANAGER: handle connection hit" << std::endl;

	struct sockaddr addr;
	socklen_t fromlen = sizeof addr;

	num_of_bytes = recvfrom(request_fd, buffer, sizeof buffer, 0, &addr, &fromlen);

}

