#include "Router_socket_manager.h"

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "router.h"
#include "Routing_table.h"
#include "Util.h"
#include "network_structures.h"


Router_socket_manager::Router_socket_manager(Router* router, const char* port) {
	this->router = router;
	this->port = new char[6];
	strcpy(this->port, port);
	this->listener = 0;
	this->addrlen = 0;
	this->res = 0;
	this->p = 0;
	this->MAXIMUM_CONNECTIONS = 1;
	//this->send_res = 0;
	//this->send_p = 0;
	//this->send_socket = 0;
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

	if (fd == listener) {
		std::cout << "ROUTER_SOCKET_MANAGER: hit listener" << std::endl;
	} else {
		std::cout << "ERROR: ROUTER_SOCKET_MANAGER: "
				<< " fd does not match listener" << std::endl;
	}

	struct sockaddr addr;
	socklen_t fromlen = sizeof addr;

	num_of_bytes = recvfrom(listener, buffer, sizeof buffer, 0, &addr, &fromlen);

	std::cout << "ROUTER_SOCKET_MANAGER: message: ";
	for(int i = 0; i < num_of_bytes; i++) {
		std::cout << std::hex << (int)buffer[i];
		std::cout << " ";
	}
	std::cout << std::dec << std::endl;

	Router_update_message message;
	Network_services::encode_router_message(&message, buffer);

	char ip_str[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &message.router_ip,
			ip_str, INET_ADDRSTRLEN);

	std::cout << "ROUTER_SOCKET_MANAGER: "
			<< " received " << num_of_bytes
			<< " bytes from " << ip_str << std::endl;

	std::cout << "DEBUG: "
			<< "num_of_fields: " << message.num_of_update_fields
			<< " router_ip: " << message.router_ip
			<< " router_port: " << message.router_port << std::endl;

	for (int i = 0; i < message.num_of_update_fields; i++) {

		Router_update_entry* entry = &message.update_entries[i];
		std::cout << "ROUTER_SOCKET_MANAGER: HANDLE_CONNECTION: "
				<< "id: " << entry->id
				<< " cost: " << entry->cost
				<< " router_ip: " << entry->router_ip
				<< " router_port: " << entry->router_port << std::endl;

	}

	router->routing_table->update_routing(&message);

	std::cout << "DEBUG: testing something" << std::endl;

}


void Router_socket_manager::broadcast_vector_table() {

	std::cout << "BROADCAST_VECTOR_TABLE:" << std::endl;

	int msg_length = 8 + 12 * router->get_routing_table_length();
	unsigned char buffer[msg_length];

	Network_services::decode_routing_table_vector(router->routing_table, buffer);

	std::vector<Routing_table_entry>* routing_table_entries =
			router->routing_table->get_my_vector()->vector_entries;

	for (std::vector<Routing_table_entry>::iterator iter = routing_table_entries->begin();
			iter != routing_table_entries->end(); iter++) {

		if ((*iter).cost == 0) continue;
		Routing_table_vector* target_vector =
				router->routing_table->get_vector((*iter).id);

		this->send_vector_table(target_vector, buffer, msg_length);

	}

}


void Router_socket_manager::send_vector_table(Routing_table_vector* dest_vector,
		const unsigned char* msg, int msg_length) {

	std::cout << "SEND_VECTOR_TABLE:" << std::endl;

	struct addrinfo send_hints, *send_res, *send_p;
	int send_socket;

	memset(&send_hints, 0, sizeof send_hints);
	send_hints.ai_family = AF_UNSPEC;
	send_hints.ai_socktype = SOCK_DGRAM;
	send_hints.ai_flags = AI_PASSIVE;

	char ip_str[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &dest_vector->router_ip,
			ip_str, INET_ADDRSTRLEN);

	char port_str[6];
	strcpy(port_str, ::toString(dest_vector->router_port).c_str());

	int rv;
	if ((rv = getaddrinfo(ip_str, port_str, &send_hints, &send_res)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}


	for(send_p = send_res; send_p != NULL; send_p = send_p->ai_next) {
		send_socket = socket(send_p->ai_family, send_p->ai_socktype, send_p->ai_protocol);
		if (send_socket < 0) {
			continue;
		}

		int yes = 1;
		setsockopt(send_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		break;
	}

	// if we got here, it means we didn't get bound
	if (send_p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	unsigned int dest_ip = dest_vector->router_ip;
	unsigned short dest_port = dest_vector->router_port;

	//memset(send_buffer, 0, 256);
	//memcpy(send_buffer, msg, msg_length);

	int sent_bytes = sendto(send_socket, msg, msg_length, 0,
			send_res->ai_addr, send_res->ai_addrlen);

	freeaddrinfo(send_res);
	// all done with this

	std::cout << "ROUTER_SOCKET_MANAGER: "
			<< " sent " << sent_bytes << " bytes to " << ip_str << std::endl;

}















