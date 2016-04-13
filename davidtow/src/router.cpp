#include <iostream>
#include <stdio.h>

#include "network_structures.h"

#include "router.h"

Router::Router(char* control_port): fdmax(0) {
	// clear the master and temp sets
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	router_socket_manager = 0;
	data_socket_manager = 0;

	control_socket_manager = new Control_socket_manager(control_port);
	register_socket(control_socket_manager);

}

Router::~Router() {
	delete control_socket_manager;
}


void Router::register_socket(Socket_manager* socket_manager) {
	socket_manager->initialize_addrinfo();
	socket_manager->create_socket();
	socket_manager->listen();

	// add to master set
	FD_SET(socket_manager->get_socketFD(), &master);

	// update max
	if (socket_manager->get_socketFD() > fdmax) {
		fdmax = socket_manager->get_socketFD();
	}
}


void Router::main() {

	printf("running main in chat server\n");

	// clear the master and read sets
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	while (1) {

		read_fds = master;

		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}

		for(int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {

				if (i == control_socket_manager->get_socketFD()) {
					control_socket_manager->handle_connection();
				}
				else if (i == router_socket_manager->get_socketFD()) {
					router_socket_manager->handle_connection();
				}
				else if (i == data_socket_manager->get_socketFD()) {
					data_socket_manager->handle_connection();
				}
				else {
					std::cout << "MAIN: error FD not detected" << std::endl;
				}

			}
		}
	}

}
