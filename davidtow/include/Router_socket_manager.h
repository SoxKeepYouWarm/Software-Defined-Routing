
#ifndef DAVIDTOW_INCLUDE_ROUTER_SOCKET_MANAGER_H
#define DAVIDTOW_INCLUDE_ROUTER_SOCKET_MANAGER_H

#include "Socket_manager.h"


class Logger;
struct Routing_table_vector;
class Router;
class Router_socket_manager: public Socket_manager {

	Router* router;
	unsigned char send_buffer[256];
	Logger* logger;

public:
	Router_socket_manager(Router* router,
			const char* port);
	void initialize_addrinfo();
	void handle_connection(int fd);
	void broadcast_vector_table();
	void send_vector_table(Routing_table_vector* dest_vector,
			const unsigned char* msg, int msg_length);
};


#endif
