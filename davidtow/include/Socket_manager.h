
#ifndef DAVIDTOW_INCLUDE_SOCKET_MANAGER_H
#define DAVIDTOW_INCLUDE_SOCKET_MANAGER_H


class Socket_manager {

public:
	virtual ~Socket_manager();
	virtual void initialize_addrinfo();
	virtual void create_socket();
	virtual void listen();

protected:
	int MAXIMUM_CONNECTIONS;
	struct addrinfo hints, *res, *p;
	int listener;
	char* port;
};


#endif
