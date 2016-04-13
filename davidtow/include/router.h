
#ifndef ROUTER_H
#define ROUTER_H

#define MAXIMUM_CONNECTIONS 6

class Router {

	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	int fdmax;        // maximum file descriptor number

	/*int listener;     // listening socket descriptor
	int newfd;        // newly accept()ed socket descriptor

	struct addrinfo hints, *ai, *p;

	char* control_port;
	char* router_port;
	char* data_port;*/

	Control_socket_manager* control_socket_manager;
	Router_socket_manager* router_socket_manager;
	Data_socket_manager* data_socket_manager;

	void set_addr_info();
	void create_control_socket();
	void listen_on_control_socket();

public:
	void main();
	Router(char* control_port);
	~Router();
};


#endif
