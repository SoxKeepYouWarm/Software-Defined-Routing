
#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

#include <set>

class Logger;

class Socket_manager {

public:
	virtual ~Socket_manager();
	virtual void create_socket();
	virtual void listen();
	virtual int get_listener_fd();
	virtual int manages_fd(int fd);

	virtual void initialize_addrinfo() = 0;
	virtual void handle_connection(int fd) = 0;

protected:
	//Router* router;
	int MAXIMUM_CONNECTIONS;
	struct addrinfo hints, *res, *p;

	std::set<int> connections;

	int listener;
	int new_fd;
	int request_fd;

	char* port;

	unsigned char buffer[1024];
	int num_of_bytes;

	struct sockaddr_storage remoteaddr;
	socklen_t addrlen;

	char remoteIP[INET6_ADDRSTRLEN];

	Logger* logger;

};


#endif
