
#ifndef CONTROL_SOCKET_MANAGER_H
#define CONTROL_SOCKET_MANAGER_H

#include "Socket_manager.h"

struct Control_message;
class Router;
class Control_socket_manager: public Socket_manager {

	Router* router;

public:
	Control_socket_manager(Router* router, char* port);
	void initialize_addrinfo();
	void handle_connection(int fd);

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
