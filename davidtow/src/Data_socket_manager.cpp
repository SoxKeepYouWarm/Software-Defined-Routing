#include "Data_socket_manager.h"

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "router.h"
#include "Routing_table.h"
#include "Util.h"


Data_socket_manager::Data_socket_manager(Router* router,
		const char* port) {
	this->router = router;
	this->port = new char[6];
	strcpy(this->port, port);
	this->listener = 0;
	this->request_fd = 0;
	this->new_fd = 0;
	this->addrlen = 0;
	this->res = 0;
	this->p = 0;
	this->MAXIMUM_CONNECTIONS = 1;
	std::cout << "DATA_SOCKET_MANAGER: initialized with port: "
			<< port << std::endl;
}


void Data_socket_manager::initialize_addrinfo() {

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int rv;
	if ((rv = getaddrinfo(NULL, port, &hints, &res)) != 0) {
		fprintf(stderr, "DATA_SOCKET_MANAGER: %s\n", gai_strerror(rv));
		exit(1);
	}

}


void Data_socket_manager::handle_data() {

	std::cout << "DATA_SOCKET_MANAGER: "
			<< "handle controller hit" << std::endl;

	if ((num_of_bytes = recv(request_fd, data_buffer, sizeof data_buffer, 0)) <= 0) {
		// got error or connection closed by client
		if (num_of_bytes == 0) {
			// connection closed
			printf("HANDLE_DATA: controller hung up\n");

		} else {
			perror("recv");
		}
		close(request_fd);
		router->unregister_fd(request_fd);
		connections.erase(request_fd);
	} else {
		printf("HANDLE_DATA: Message received was: %s\n", data_buffer);

		// handle incoming data

		Data_packet message;
		Network_services::encode_data_message(&message, data_buffer);

		//handle_control_message(&message);

		memset(&data_buffer, 0, sizeof data_buffer);

	}

}


void Data_socket_manager::handle_listener() {

	std::cout << "DATA_SOCKET_MANAGER: "
			<< "handle listener hit" << std::endl;

	addrlen = sizeof remoteaddr;
	new_fd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
	if (new_fd == -1) {
		perror("accept");
		exit(1);
	}

	std::cout <<"DATA_SOCKET_MANAGER: "
			<< "socket accepted" << std::endl;

	connections.insert(new_fd);
	router->register_fd(new_fd);

}


void Data_socket_manager::handle_connection(int fd) {
	std::cout << "DATA_SOCKET_MANAGER: handle connection hit" << std::endl;

	request_fd = fd;
	if (fd == listener) {
		handle_listener();
	} else {
		handle_data();
	}

}



void Data_socket_manager::send_data(Data_packet* data) {

	// send data to target
	int msg_size = 1024 + 12;
	unsigned char message[msg_size];
	Network_services::decode_data_message(data, message);


	unsigned int data_ip = data->destination_router_ip;
	unsigned int destination_router_id =
			router->routing_table->get_routerId_from_ip(data->destination_router_ip);

	unsigned int next_hop_id =
			router->routing_table->get_next_hop_routerId(destination_router_id);

	unsigned int next_hop_router_ip =
			router->routing_table->get_vector(next_hop_id)->router_ip;
	unsigned int next_hop_router_port =
			router->routing_table->get_vector(next_hop_id)->data_port;

	std::cout << "SEND_DATA:" << std::endl;

	struct addrinfo send_hints, *send_res, *send_p;
	int send_socket;

	memset(&send_hints, 0, sizeof send_hints);
	send_hints.ai_family = AF_UNSPEC;
	send_hints.ai_socktype = SOCK_STREAM;
	send_hints.ai_flags = AI_PASSIVE;

	char ip_str[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &next_hop_router_ip,
			ip_str, INET_ADDRSTRLEN);

	char port_str[6];
	strcpy(port_str, ::toString(next_hop_router_port).c_str());

	int rv;
	if ((rv = getaddrinfo(ip_str, port_str, &send_hints, &send_res)) != 0) {
		fprintf(stderr, "data_socket:send_data: %s\n", gai_strerror(rv));
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

	if (connect(send_socket, send_res->ai_addr, send_res->ai_addrlen) < 0){
		std::cerr << "DATA_SOCKET_MANAGER: error on connect" << std::endl;
	} else {

		std::cout << "DATA_SOCKET_MANAGER: connected send_socket" << std::endl;



	}


	int sent_bytes = send(send_socket, message, msg_size, 0);

	freeaddrinfo(send_res);
	// all done with this

	std::cout << "DATA_SOCKET_MANAGER: "
			<< " sent " << sent_bytes << " bytes to " << ip_str << std::endl;



	delete data;
}










