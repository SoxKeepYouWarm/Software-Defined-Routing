#include "Data_socket_manager.h"

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>

#include "router.h"


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

	if ((num_of_bytes = recv(request_fd, buffer, sizeof buffer, 0)) <= 0) {
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
		printf("HANDLE_DATA: Message received was: %s\n", buffer);

		// handle incoming data

		Data_packet message;
		Network_services::encode_data_message(&message, buffer);

		//handle_control_message(&message);

		memset(&buffer, 0, sizeof buffer);

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

	}

}



void Data_socket_manager::send_data(Data_packet* data) {

	// TODO send data to target

	delete data;
}










