#include "Socket_manager.h"

#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>


Socket_manager::~Socket_manager() {

}


void Socket_manager::create_socket() {

	for(p = res; p != NULL; p = p->ai_next) {
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) {
			continue;
		}

		int yes = 1;
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		struct sockaddr_in* addr = (struct sockaddr_in*)p->ai_addr;

		char str[INET_ADDRSTRLEN];

		inet_ntop(AF_INET, &addr->sin_addr,
				str, INET_ADDRSTRLEN);

		std::cout << "BIND: binded to ip: " << str << std::endl;

		break;
	}

	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	freeaddrinfo(res);
	// all done with this

	std::cout << "CREATE_SOCKET: "
			<< " created socket successfully" << std::endl;

	connections.insert(listener);

}


void Socket_manager::listen() {

	if (::listen(listener, MAXIMUM_CONNECTIONS) == -1) {
		perror("listen");
		exit(3);
	}

	std::cout << "LISTEN: FD: " << listener << std::endl;

}


int Socket_manager::get_listener_fd() {
	return this->listener;
}


int Socket_manager::manages_fd(int fd) {
	return connections.find(fd) != connections.end();
}



