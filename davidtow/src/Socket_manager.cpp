#include <iostream>
#include <stdio.h>

#include "Socket_manager.h"


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


