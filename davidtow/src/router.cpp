#include "router.h"

#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>

#include "network_structures.h"
#include "Control_socket_manager.h"
#include "Router_socket_manager.h"
#include "Data_socket_manager.h"
#include "Timer.h"
#include "Routing_table.h"


Router::Router(char* control_port): fdmax(0), is_running_timer(0),
			routing_table(0),
			router_socket_manager(0), data_socket_manager(0) {

	// clear the master and temp sets
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	timer = new Timer(this);

	control_socket_manager = new Control_socket_manager(this, control_port);
	control_socket_manager->initialize_addrinfo();
	control_socket_manager->create_socket();
	control_socket_manager->listen();
	register_fd(control_socket_manager->get_listener_fd());
}

Router::~Router() {

	delete control_socket_manager;
	delete router_socket_manager;
	delete data_socket_manager;

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

		int select_value;
		if (is_running_timer) {
			select_value = select(fdmax+1, &read_fds, NULL, NULL, &tv);
		} else {
			select_value = select(fdmax+1, &read_fds, NULL, NULL, NULL);
		}

		if (select_value == -1) {
			perror("select");
			exit(4);
		} else if (select_value == 0) {
			// timer just expired
			std::cout << "MAIN: timer just expired" << std::endl;
			// TODO: handle timeout
		}

		for(int i = 0; i <= fdmax; i++) {

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

	if (routing_table != 0) {
		std::cout << "BUILD_ROUTING_TABLE: ERROR: "
				<< "routing table was already initialized"
				<< std::endl;
	}

	routing_table = new Routing_table(init_payload);

	std::cout << "BUILD_ROUTING_TABLE: new routing table built"
			<< std::endl;


	// routing table is setup, time to start the timer
	timer->start(init_payload->update_period, routing_table->my_router_id);
	this->is_running_timer = 1;

}


int Router::get_my_router_id() {
	if (this->routing_table) return routing_table->my_router_id;
	else {
		std::cout << "ERROR: get_my_router_id: "
				<< "routing table wasn't initialized" << std::endl;
		return -1;
	}
}


int Router::get_routing_table_length() {
	if (this->routing_table) return routing_table->routing_table_length;
	else {
		std::cout << "ERROR: get_routing_table_length: "
				<< "routing table wasn't initialized" << std::endl;
		return -1;
	}
}


const std::vector< std::vector<Routing_table_entry> >* Router::get_routing_table() {
	if (this->routing_table) return routing_table->get_routing_table();
	else {
		std::cout << "ERROR: get_routing_table: "
				<< "routing table wasn't initialized" << std::endl;
		return 0;
	}
}


std::vector< std::vector<Routing_table_entry> >* Router::get_writeable_routing_table() {
	if (this->routing_table) return routing_table->get_writeable_routing_table();
	else {
		std::cout << "ERROR: get_writeable_routing_table: "
				<< "routing table wasn't initialized" << std::endl;
		return 0;
	}
}











