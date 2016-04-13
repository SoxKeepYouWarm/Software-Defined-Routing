
#ifndef DAVIDTOW_INCLUDE_SOCKET_MANAGER_H
#define DAVIDTOW_INCLUDE_SOCKET_MANAGER_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

class Socket_manager {

public:
	virtual ~Socket_manager();
	virtual void initialize_addrinfo() = 0;
	virtual void create_socket();
	virtual void listen();
	virtual int get_socketFD();
	virtual void handle_connection() = 0;

protected:
	int MAXIMUM_CONNECTIONS;
	struct addrinfo hints, *res, *p;
	int listener;
	char* port;
};


#endif
