
#ifndef DAVIDTOW_INCLUDE_DATA_SOCKET_MANAGER_H
#define DAVIDTOW_INCLUDE_DATA_SOCKET_MANAGER_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "Socket_manager.h"

class Router;
class Data_socket_manager: public Socket_manager {

	Router* router;

public:
	Data_socket_manager(Router* router, char* port);
	void initialize_addrinfo();
	void handle_connection(int fd);
	int manages_fd(int fd);
};


#endif
