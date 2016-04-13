#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../include/router.h"
#include "../include/network_structures.h"
#include "../include/Control_socket_manager.h"
#include "../include/Router_socket_manager.h"
#include "../include/Data_socket_manager.h"


Router::Router(char* control_port): fdmax(0),
			router_socket_manager(0), data_socket_manager(0){
	// clear the master and temp sets
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	control_socket_manager = new Control_socket_manager(control_port);


}

Router::~Router() {
	delete control_socket_manager;
}



void Router::main() {

	printf("running main in chat server\n");

	// clear the master and read sets
	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	while (1) {

		read_fds = master;

		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}

		for(int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == STDIN) {
					printf("hit stdin handler\n");
					stdin_handler();

				} else if (i == listener) {
					printf("hit listener handler\n");
					listener_handler();

				} else {
					printf("hit client handler\n");
					client_handler(i);

				}
			}
		}
	}

}
