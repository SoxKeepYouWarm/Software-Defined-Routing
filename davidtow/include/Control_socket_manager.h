
#ifndef DAVIDTOW_INCLUDE_CONTROL_SOCKET_MANAGER_H
#define DAVIDTOW_INCLUDE_CONTROL_SOCKET_MANAGER_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "Socket_manager.h"

class Router;
class Control_socket_manager: public Socket_manager {

	int control_fd;
	Router* router;

public:
	Control_socket_manager(Router* router, char* port);
	void initialize_addrinfo();
	void handle_connection(int fd);
	int get_control_socket_fd();
	int manages_fd(int fd);
	void handle_listener();
	void handle_controller();
};


#endif
