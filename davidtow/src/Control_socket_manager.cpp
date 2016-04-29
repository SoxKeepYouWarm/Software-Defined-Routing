#include "Control_socket_manager.h"

#include <iostream>
#include <cstring>
#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>

#include "network_structures.h"
#include "router.h"
#include "Routing_table.h"


Control_socket_manager::Control_socket_manager(Router* router, char* port) {
	this->router = router;
	this->port = port;
	this->listener = 0;
	this->new_fd = 0;
	this->request_fd = 0;
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
	new_fd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
	if (new_fd == -1) {
		perror("accept");
		exit(1);
	}

	std::cout <<"CONTROL_SOCKET_MANAGER: "
			<< "socket accepted" << std::endl;

	connections.insert(new_fd);
	router->register_fd(new_fd);

}


void Control_socket_manager::send_empty_response(Control_message* message) {

	message->header.payload_length = 0;
	message->header.response_time = 0;

	int response_size = 8;
	unsigned char response[response_size];
	Network_services::decode_control_message(message, response);
	Network_services::send(request_fd, response, (size_t) response_size);

}


void Control_socket_manager::handle_author(Control_message* message) {

	const char payload[] = "I, davidtow, "
			"have read and understood "
			"the course academic integrity policy.";

	unsigned short payload_length = sizeof(payload) / sizeof(payload[0]);

	message->header.payload_length = payload_length;
	message->header.response_time = 0;
	message->payload = (void*)payload;

	int response_size = 8 + payload_length;
	unsigned char response[response_size];
	Network_services::decode_control_message(message, response);

	std::cout << "HANDLE_AUTHOR: response length is: "
			<< response_size
			<< std::endl;

	printf("HANDLE_AUTHOR: response is: %.*s\n",
			ntohs(message->header.payload_length), (char*)message->payload);

	Network_services::send(request_fd, response, (size_t) response_size);

}


void Control_socket_manager::handle_init(Control_message* message) {

	std::cout << "HANDLE_INIT: payload_length: "
			<< message->header.payload_length << std::endl;

	Control_message_init_payload* payload =
			(Control_message_init_payload*)message->payload;

	std::cout << "number of routers: " << payload->number_of_routers
			<< " update period: " << payload->update_period << std::endl;

	for (int i = 0; i < payload->number_of_routers; i++) {
		Init_payload_router_entry* iter = payload->entry_list + i;

		std::cout << "router entry: "
				<< " id: " << iter->id
				<< " router_port: " << iter->router_port
				<< " data_port: " << iter->data_port
				<< " cost: " << iter->cost
				<< " router_ip " << iter->router_ip
				<< std::endl;
	}

	this->router->build_routing_table(payload);

	send_empty_response(message);

}


void Control_socket_manager::handle_routing_table(Control_message* message) {

	std::cout << "HANDLE_ROUTING_TABLE" << std::endl;

	if (router->get_routing_table() == 0) {
		std::cout << "HANDLE_ROUTING_TABLE: "
				<< "routing table was not initialized, aborting" << std::endl;
		return;
	}

	message->header.payload_length = (8 * router->get_routing_table_length());
	message->header.response_time = 0;

	std::cout << "HANDLE_ROUTING_TABLE: "
			<< "payload_length: " << message->header.payload_length << std::endl;

	int response_size = 8 + message->header.payload_length;
	unsigned char response[response_size];
	Network_services::decode_control_message_routing_table(message,
			router->get_routing_table(), router->get_my_router_id(), response);
	Network_services::send(request_fd, response, (size_t) response_size);


}


void Control_socket_manager::handle_update(Control_message* message) {

	Control_message_update_payload* payload =
			(Control_message_update_payload*) message->payload;

	router->routing_table->update(payload->router_id, payload->cost);

	std::cout << "HANDLE_UPDATE: "
			<< "router_id: " << payload->router_id
			<< " new cost: " << payload->cost << std::endl;

	send_empty_response(message);

}


void Control_socket_manager::handle_crash(Control_message* message) {

	send_empty_response(message);

	std::cout << "HANDLE_CRASH: exiting" << std::endl;

	exit(0);

}


void Control_socket_manager::handle_sendfile(Control_message* message) {

	std::cout << "HANDLE_SENDFILE: " << std::endl;

	send_empty_response(message);

}


void Control_socket_manager::handle_sendfile_stats(Control_message* message) {

	std::cout << "HANDLE_SENDFILE_STATS: " << std::endl;

}


void Control_socket_manager::handle_last_data_packet(Control_message* message) {

	std::cout << "HANDLE_LAST_DATA_PACKET: " << std::endl;

}


void Control_socket_manager::handle_penultimate_data_packet(Control_message* message) {

	std::cout << "HANDLE_PENULTIMATE_DATA_PACKET: " << std::endl;

}


void Control_socket_manager::set_message_destination_ip(Control_message* message) {

	uint32_t net_ip = htonl(((in_addr*)
			Network_services::get_in_addr((struct sockaddr*)&remoteaddr))->s_addr);

	std::cout << "SET_MESSAGE_DESTINATION_IP: remote ip: "
			<< net_ip << std::endl;

	memcpy(&(message->header.destination_router_ip), &net_ip, 4);

}


void Control_socket_manager::handle_control_message(Control_message* message) {

	set_message_destination_ip(message);

	const int code = message->header.control_code;
	switch (code) {

	case AUTHOR:
		handle_author(message);
		break;
	case INIT:
		handle_init(message);
		break;
	case ROUTING_TABLE:
		handle_routing_table(message);
		break;
	case UPDATE:
		handle_update(message);
		break;
	case CRASH:
		handle_crash(message);
		break;
	case SENDFILE:
		handle_sendfile(message);
		break;
	case SENDFILE_STATS:
		handle_sendfile_stats(message);
		break;
	case LAST_DATA_PACKET:
		handle_last_data_packet(message);
		break;
	case PENULTIMATE_DATA_PACKET:
		handle_penultimate_data_packet(message);
		break;
	}

}


void Control_socket_manager::handle_controller() {

	std::cout << "CONTROL_SOCKET_MANAGER: "
				<< "handle controller hit" << std::endl;

	if ((num_of_bytes = recv(request_fd, buffer, sizeof buffer, 0)) <= 0) {
		// got error or connection closed by client
		if (num_of_bytes == 0) {
			// connection closed
			printf("HANDLE_CONTROLLER: controller hung up\n");

		} else {
			perror("recv");
		}
		close(request_fd);
		router->unregister_fd(request_fd);
		connections.erase(request_fd);
	} else {
		printf("HANDLE_CONTROLLER: Message received was: %s\n", buffer);

		// handle incoming data

		Control_message message;
		Network_services::encode_control_message(&message, buffer);
		handle_control_message(&message);

		memset(&buffer, 0, sizeof buffer);

	}

}


void Control_socket_manager::handle_connection(int fd) {

	request_fd = fd;
	if (fd == listener) {
		handle_listener();
	} else {
		handle_controller();
	}

}


