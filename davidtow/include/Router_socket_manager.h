
#ifndef DAVIDTOW_INCLUDE_ROUTER_SOCKET_MANAGER_H
#define DAVIDTOW_INCLUDE_ROUTER_SOCKET_MANAGER_H


class Router_socket_manager {

	int MAXIMUM_CONNECTIONS = 5;

	struct addrinfo hints, *res, *p;
	int listener;
	char* port;

public:
	Router_socket_manager(char* port);
	void initialize_addrinfo();
	void create_socket();
	void listen();
};


#endif
