
#ifndef DAVIDTOW_INCLUDE_ROUTER_SOCKET_MANAGER_H
#define DAVIDTOW_INCLUDE_ROUTER_SOCKET_MANAGER_H

#include "Socket_manager.h"

class Router_socket_manager: public Socket_manager {

public:
	Router_socket_manager(char* port);
	void initialize_addrinfo();
	void handle_connection();
};


#endif
