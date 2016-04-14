
#ifndef DAVIDTOW_INCLUDE_ROUTER_SOCKET_MANAGER_H
#define DAVIDTOW_INCLUDE_ROUTER_SOCKET_MANAGER_H

#include "Socket_manager.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

class Router_socket_manager: public Socket_manager {

public:
	Router_socket_manager(Router* router, char* port);
	void initialize_addrinfo();
	void handle_connection();
};


#endif
