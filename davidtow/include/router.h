
#ifndef ROUTER_H
#define ROUTER_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <vector>

//#include "Socket_manager.h"
#include "Control_socket_manager.h"
#include "Router_socket_manager.h"
#include "Data_socket_manager.h"

class Router {

	fd_set master;    // master file descriptor list
	fd_set read_fds;  // temp file descriptor list for select()
	int fdmax;        // maximum file descriptor number

	Control_socket_manager* control_socket_manager;
	Router_socket_manager* router_socket_manager;
	Data_socket_manager* data_socket_manager;

	std::string toString(int val);

public:
	Router(char* control_port);
	~Router();
	void main();
	void register_fd(int fd);
	void unregister_fd(int fd);
	void build_routing_table(
		Control_message_init_payload* init_payload);

	std::vector<std::vector
		<Routing_table_entry> >* routing_table;

	int routing_table_length;
	int router_id;
};


#endif
