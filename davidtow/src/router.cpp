#include <iostream>
#include <stdio.h>

#include "network_structures.h"

#include "router.h"

Router::Router(char* control_port): fdmax(0) {
	// clear the master and temp sets
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	router_id = -1;

	router_socket_manager = 0;
	data_socket_manager = 0;

	control_socket_manager = new Control_socket_manager(this, control_port);
	control_socket_manager->initialize_addrinfo();
	control_socket_manager->create_socket();
	control_socket_manager->listen();
	register_fd(control_socket_manager->get_listener_fd());
}

Router::~Router() {
	delete control_socket_manager;
}


void Router::register_fd(int fd) {

	// add to master set
	FD_SET(fd, &master);

	std::cout << "MAIN: new socket FD is: "
			<< fd << std::endl;

	// update max
	if (fd > fdmax) {
		fdmax = fd;
		std::cout << "REGISTER_SOCKET: new fdmax is: "
				<< fdmax << std::endl;
	}
}


void Router::unregister_fd(int fd) {
	FD_CLR(fd, &master);
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
				if (control_socket_manager->manages_fd(i)) {
					control_socket_manager->handle_connection(i);
				}
				else if (router_socket_manager->manages_fd(i)) {
					router_socket_manager->handle_connection(i);
				}
				else if (data_socket_manager->manages_fd(i)) {
					data_socket_manager->handle_connection(i);
				}
				else {
					std::cout << "MAIN: error FD not detected" << std::endl;
				}

			}
		}
	}

}
