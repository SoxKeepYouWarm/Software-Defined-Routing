
#ifndef DATA_SOCKET_MANAGER_H
#define DATA_SOCKET_MANAGER_H

#include "Socket_manager.h"

class Router;
class Data_socket_manager: public Socket_manager {

	Router* router;

public:
	Data_socket_manager(Router* router,
			const char* port);
	void initialize_addrinfo();
	void handle_connection(int fd);

	void handle_listener();
	void handle_data();
};


#endif
