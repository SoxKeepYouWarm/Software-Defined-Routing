#include <iostream>

#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>

#include "router.h"
#include "Router_socket_manager.h"


Router_socket_manager::Router_socket_manager(Router* router, char* port) {
	this->router = router;
	this->port = port;
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
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int rv;
	if ((rv = getaddrinfo(NULL, port, &hints, &res)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}

}


void Router_socket_manager::handle_connection(int fd) {
	std::cout << "ROUTER_SOCKET_MANAGER: handle connection hit" << std::endl;
}


