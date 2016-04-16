#include <iostream>

#include <cstring>
#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>

#include "network_structures.h"
#include "router.h"
#include "Control_socket_manager.h"


Control_socket_manager::Control_socket_manager(Router* router, char* port) {
	this->router = router;
	this->port = port;
	this->listener = 0;
	this->control_fd = 0;
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
	control_fd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
	if (control_fd == -1) {
		perror("accept");
		exit(1);
	}

	std::cout <<"CONTROL_SOCKET_MANAGER: "
			<< "socket accepted" << std::endl;

	router->register_fd(control_fd);

}


// get sockaddr, IPv4 or IPv6:
void* get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void Control_socket_manager::handle_author(Control_message* message) {

	const char payload[] = "I, davidtow, "
			"have read and understood "
			"the course academic integrity policy.";

	unsigned short length = sizeof(payload) / sizeof(payload[0]);
	unsigned short net_length = htons(length);
	memcpy(&(message->header.payload_length), &net_length, 2);

	unsigned char response_code = 0;
	memcpy(&(message->header.response_time), &response_code, 1);

	memcpy(&(message->payload), payload, length);

	int response_size = 8 + ntohs(message->header.payload_length);
	char response[response_size];
	decode_control_message(message, response, response_size);

	std::cout << "HANDLE_AUTHOR: response length is: "
			<< response_size
			<< std::endl;

	printf("HANDLE_AUTHOR: response is: %.*s\n",
			ntohs(message->header.payload_length), (char*)message->payload);

	send(request_fd, response, (size_t) response_size);

}


void Control_socket_manager::handle_init(Control_message* message) {

	// each router entry is 16 bytes long
	unsigned short num_of_routers_bytes = 0;
	memcpy(&num_of_routers_bytes, message->payload, 2);
	int num_of_routers = ntohs(num_of_routers_bytes);

	std::cout << "HANDLE_INIT: payload_length: "
			<< message->header.payload_length
			<< " num_of_routers " << num_of_routers << std::endl;

	Control_message_init_payload payload;
	for (int i = 0; i < num_of_routers; i++) {
		int offset = (i * 12) + 2;

	}

}


void Control_socket_manager::handle_routing_table(Control_message* message) {

}


void Control_socket_manager::handle_update(Control_message* message) {

}


void Control_socket_manager::handle_crash(Control_message* message) {

}


void Control_socket_manager::handle_sendfile(Control_message* message) {

}


void Control_socket_manager::handle_sendfile_stats(Control_message* message) {

}


void Control_socket_manager::handle_last_data_packet(Control_message* message) {

}


void Control_socket_manager::handle_penultimate_data_packet(Control_message* message) {

}


void Control_socket_manager::set_message_destination_ip(Control_message* message) {

	uint32_t net_ip = htonl(((in_addr*)
			::get_in_addr((struct sockaddr*)&remoteaddr))->s_addr);

	std::cout << "SET_MESSAGE_DESTINATION_IP: remote ip: %d"
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

	if ((num_of_bytes = recv(control_fd, buffer, sizeof buffer, 0)) <= 0) {
			// got error or connection closed by client
			if (num_of_bytes == 0) {
				// connection closed
				printf("HANDLE_CONTROLLER: controller hung up\n");

			} else {
				perror("recv");
			}
			close(control_fd);
			router->unregister_fd(control_fd);
		} else {
			printf("HANDLE_CONTROLLER: Message received was: %s\n", buffer);

			// handle incoming data
			Control_message message = encode_control_message();
			handle_control_message(&message);

			memset(&buffer, 0, sizeof buffer);

		}

}


Control_message Control_socket_manager::encode_control_message() {

	Control_message message;
	memcpy(message.header.destination_router_ip, buffer, 4);
	memcpy(&message.header.control_code, buffer + 4, 1);
	memcpy(&message.header.response_time, buffer + 5, 1);
	memcpy(&message.header.payload_length, buffer + 6, 2);

	// check if message contains a payload
	if (message.header.payload_length > 0) {
		memcpy(message.payload, buffer + 8, num_of_bytes - 8);
	}
	return message;

}


void Control_socket_manager::decode_control_message(Control_message* message,
		char* response,
		int response_size) {

	memcpy(response, &message->header.destination_router_ip, 4);
	memcpy(response + 4, &message->header.control_code, 1);
	memcpy(response + 5, &message->header.response_time, 1);
	memcpy(response + 6, &message->header.payload_length, 2);
	if (ntohs(message->header.payload_length > 0)) {
		memcpy(response + 8, message->payload,
				ntohs(message->header.payload_length));
	}

}


void Control_socket_manager::handle_connection(int fd) {

	request_fd = fd;
	if (fd == listener) handle_listener();
	if (fd == control_fd) handle_controller();

}


void Control_socket_manager::send(int fd, char* message, size_t size) {


	if (int out_bytes = ::send(fd, message, size, 0) == -1) {
		perror("send");
	} else {
		printf("SEND: response sent %d bytes successfully\n", out_bytes);
	}

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

