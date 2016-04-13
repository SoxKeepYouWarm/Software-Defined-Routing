
#ifndef DAVIDTOW_INCLUDE_CONTROL_SOCKET_MANAGER_H
#define DAVIDTOW_INCLUDE_CONTROL_SOCKET_MANAGER_H


class Control_socket_manager {

	int MAXIMUM_CONNECTIONS = 1;

	struct addrinfo hints, *res, *p;
	int listener;
	char* port;

public:
	Control_socket_manager(char* port);
	void initialize_addrinfo();
	void create_socket();
	void listen();
};



#endif
