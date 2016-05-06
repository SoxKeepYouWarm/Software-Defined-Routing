
#ifndef DATA_SOCKET_MANAGER_H
#define DATA_SOCKET_MANAGER_H

#include "Socket_manager.h"

struct Data_packet;
class Router;
class Data_socket_manager: public Socket_manager {

	Router* router;

	unsigned char incoming_buffer[1036];
	struct addrinfo incoming_hints, *incoming_res, *incoming_p;
	int incoming_socket;

	unsigned char outgoing_buffer[1036];
	struct addrinfo outgoing_hints, *outgoing_res, *outgoing_p;
	int outgoing_socket;

	//std::vector<char*> message_chunk_buffer;

public:
	Data_socket_manager(Router* router,
			const char* port);
	void initialize_addrinfo();
	void handle_connection(int fd);

	void handle_listener();
	void handle_data();

	void initialize_connection(unsigned int ip);
	void close_connection();
	void send_data(Data_packet* payload);
	void write_data_to_file(Data_packet* data);
};


#endif
