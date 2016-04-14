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

	std::cout << "MAIN: new socket FD is: "
			<< socket_manager->get_socketFD() << std::endl;

	// update max
	if (socket_manager->get_socketFD() > fdmax) {
		fdmax = socket_manager->get_socketFD();
		std::cout << "REGISTER_SOCKET: new fdmax is: "
				<< fdmax << std::endl;
	}
}


void Router::main() {

	std::cout << "running main in chat server\n" << std::endl;

	while (1) {

		std::cout << "MAIN: entered main loop" << std::endl;

		read_fds = master;

		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}

		for(int i = 0; i <= fdmax; i++) {

			std::cout << "MAIN: looping over FD's" << std::endl;

			if (FD_ISSET(i, &read_fds)) {

				std::cout << "MAIN: FD_ISSET was hit" << std::endl;
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
