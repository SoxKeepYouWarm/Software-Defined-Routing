
#ifndef DAVIDTOW_INCLUDE_DATA_SOCKET_MANAGER_H
#define DAVIDTOW_INCLUDE_DATA_SOCKET_MANAGER_H


class Data_socket_manager: public Socket_manager {

public:
	Data_socket_manager(char* port);
	void initialize_addrinfo();
};


#endif
