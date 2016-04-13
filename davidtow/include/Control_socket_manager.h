
#ifndef DAVIDTOW_INCLUDE_CONTROL_SOCKET_MANAGER_H
#define DAVIDTOW_INCLUDE_CONTROL_SOCKET_MANAGER_H


class Control_socket_manager: public Socket_manager {

public:
	Control_socket_manager(char* port);
	void initialize_addrinfo();
};


#endif
