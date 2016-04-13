
#ifndef DAVIDTOW_INCLUDE_DATA_SOCKET_MANAGER_H
#define DAVIDTOW_INCLUDE_DATA_SOCKET_MANAGER_H

#include "Socket_manager.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

class Data_socket_manager: public Socket_manager {

public:
	Data_socket_manager(char* port);
	void initialize_addrinfo();
	void handle_connection();
};


#endif
