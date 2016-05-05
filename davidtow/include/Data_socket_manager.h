
#ifndef DATA_SOCKET_MANAGER_H
#define DATA_SOCKET_MANAGER_H

#include "Socket_manager.h"

struct Data_packet;
class Router;
class Data_socket_manager: public Socket_manager {

	Router* router;

	unsigned char data_buffer[2048];

public:
	Data_socket_manager(Router* router,
			const char* port);
	void initialize_addrinfo();
	void handle_connection(int fd);

	void handle_listener();
	void handle_data();

	void send_data(Data_packet* data);
	void write_data_to_file(Data_packet* data);
};


#endif
