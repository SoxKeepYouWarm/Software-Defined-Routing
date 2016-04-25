#include <iostream>
#include <stdio.h>

#include "network_structures.h"

#include "router.h"

Router::Router(char* control_port): fdmax(0) {
	// clear the master and temp sets
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	router_id = -1;
	routing_table = 0;
	routing_table_length = 0;

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
	/*
	for (int i = 0; i < routing_table_length; i++) {
		delete[] routing_table[i];
	}
	delete[] routing_table;
	*/
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

			//std::cout << "MAIN: looping over FD's" << std::endl;

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



void Router::build_routing_table(Control_message_init_payload* init_payload) {


	std::cout << "DEBUG: reached build_routing_table" << std::endl;

	if (this->router_id != -1) {
		std::cout << "BUILD_ROUTING_TABLE: ERROR: "
				<< "routing table was already initialized"
				<< std::endl;
	}

	routing_table_length = init_payload->number_of_routers;
	routing_table = new std::vector< std::vector<int> >
			(routing_table_length + 1, std::vector<int>(routing_table_length + 1, -1));

	std::vector<int> my_vector(routing_table_length + 1);

	for (int i = 0; i < routing_table_length; i++) {

		Init_payload_router_entry* entry = (init_payload->entry_list + i);

		my_vector.at(entry->id) = entry->cost;
		if (entry->cost == 0) {
			this->router_id = entry->id;
		}

	}

	if (this->router_id == -1) {
		std::cout << "BUILD_ROUTING_TABLE: ERROR: "
				<< "couldn't find a routing entry id with cost 0"
				<< std::endl;
		exit(4);
	}


	for (int i = 1; i < routing_table_length + 1; i++) {
		routing_table->at(router_id).at(i) = my_vector[i];
	}


	std::cout << "BUILD_ROUTING_TABLE: new routing table"
			<< std::endl;

	std::cout << "ROUTER_ID: " << router_id << std::endl;

	for (int i = 1; i < routing_table_length + 1; i++) {

		std::cout << "id: " << i
			<< " cost: " << routing_table->at(router_id).at(i)
			<< std::endl;

	}

}
















