#include <iostream>

#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>

#include "router.h"
#include "Control_socket_manager.h"


Control_socket_manager::Control_socket_manager(Router* router, char* port) {
	this->router = router;
	this->port = port;
	this->listener = 0;
	this->control_fd = 0;
	this->addrlen = 0;
	this->res = 0;
	this->p = 0;
	this->MAXIMUM_CONNECTIONS = 1;
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


void Control_socket_manager::handle_listener() {

	std::cout << "CONTROL_SOCKET_MANAGER: "
			<< "handle listener hit" << std::endl;

	addrlen = sizeof remoteaddr;
	control_fd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
	if (control_fd == -1) {
		perror("accept");
		exit(1);
	}

	std::cout <<"CONTROL_SOCKET_MANAGER: "
			<< "socket accepted" << std::endl;

	router->register_fd(control_fd);

}


void Control_socket_manager::handle_controller() {

	std::cout << "CONTROL_SOCKET_MANAGER: "
				<< "handle controller hit" << std::endl;

	if ((num_of_bytes = recv(control_fd, buffer, sizeof buffer, 0)) <= 0) {
			// got error or connection closed by client
			if (num_of_bytes == 0) {
				// connection closed
				printf("CONTROL_SOCKET_MANAGER: controller hung up\n");

			} else {
				perror("recv");
			}
			close(control_fd);
			router->unregister_fd(control_fd);
		} else {
			printf("Received %d bytes\n", num_of_bytes);
			printf("Message received was: %s\n", buffer);

			// handle incoming data

			memset(&buffer, 0, sizeof buffer);
		}

}

void Control_socket_manager::handle_connection(int fd) {

	if (fd == listener) handle_listener();
	if (fd == control_fd) handle_controller();

}


int Control_socket_manager::get_control_socket_fd() {
	return this->control_fd;
}


int Control_socket_manager::manages_fd(int fd) {
	if (fd == control_fd || fd == listener) {
		return 1;
	} else {
		return 0;
	}
}

