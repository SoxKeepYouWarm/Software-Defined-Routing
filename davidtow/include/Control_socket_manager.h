
#ifndef DAVIDTOW_INCLUDE_CONTROL_SOCKET_MANAGER_H
#define DAVIDTOW_INCLUDE_CONTROL_SOCKET_MANAGER_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "Socket_manager.h"
#include "network_structures.h"


class Router;
class Control_socket_manager: public Socket_manager {

	int control_fd;
	int request_fd;
	Router* router;

public:
	Control_socket_manager(Router* router, char* port);
	void initialize_addrinfo();
	void handle_connection(int fd);
	int get_control_socket_fd();
	int manages_fd(int fd);

	void handle_listener();
	void handle_controller();

	void send_empty_response(Control_message* message);

	void handle_control_message(Control_message* message);

	void set_message_destination_ip(Control_message* message);

	void handle_author(Control_message* message);
	void handle_init(Control_message* message);
	void handle_routing_table(Control_message* message);
	void handle_update(Control_message* message);
	void handle_crash(Control_message* message);
	void handle_sendfile(Control_message* message);
	void handle_sendfile_stats(Control_message* message);
	void handle_last_data_packet(Control_message* message);
	void handle_penultimate_data_packet(Control_message* message);
};


#endif
