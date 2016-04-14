#include <iostream>

#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>


#include "Control_socket_manager.h"


Control_socket_manager::Control_socket_manager(char* port) {
	this->port = port;
	this->listener = 0;
	this->res = 0;
	this->p = 0;
	this->MAXIMUM_CONNECTIONS = 2;
	std::cout << "CONTROL_SOCKET_MANAGER: initialized with port: "
			<< port << std::endl;
}


void Control_socket_manager::initialize_addrinfo() {

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int rv;
	if ((rv = getaddrinfo(NULL, port, &hints, &res)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}

	std::cout << "CONTROL_SOCKET_MANAGER: "
			<< "initialized address info successfully" << std::endl;

}


void Control_socket_manager::handle_connection() {
	std::cout << "CONTROL_SOCKET_MANAGER: "
			<< "handle connection hit" << std::endl;

}



