
#include <iostream>

#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFERSIZE              100
#define MAXIMUM_CONNECTIONS     10
#define PORT                    "5000"

int main() {

    printf("running main in chat server\n");
	
	char input[BUFFERSIZE];
	
	int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;
	
	char remoteIP[INET6_ADDRSTRLEN];
	int remote_port;
	
	fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number
    
	struct addrinfo hints, *ai, *p;
	
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	
    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int rv;
	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}
	
	
    for(p = ai; p != NULL; p = p->ai_next) {
    	listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) { 
			continue;
		}
		
		int yes=1;        
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}
	
	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this



    if (listen(listener, MAXIMUM_CONNECTIONS) == -1) {
        perror("listen");
        exit(3);
    }


    FD_SET(listener, &master);
    fdmax = listener; 



    while (1) {
		
        read_fds = master; 
        
        int sel_val = select(fdmax+1, &read_fds, NULL, NULL, &tv);
        
		if (sel_val == -1) {
            perror("select");
            exit(4);
        } else if (sel_val == 0) {
            printf("Select timed out\n");
            tv.tv_sec = 10;
		    tv.tv_usec = 0;
        }

        for(int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { 
                if (i == listener) {
                    printf("hit listener\n");
                    addrlen = sizeof remoteaddr;
	                newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
	                
	                printf("time: sec: %ld m_sec: %ld", tv.tv_sec, tv.tv_usec);
	                tv.tv_sec = 10;
		            tv.tv_usec = 0;
	                
	                if (newfd == -1) {
    	                perror("accept");
                    } else {
		
    	                FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
			                fdmax = newfd;
		                }
                
                    } 
                    
                } else if (i == newfd) {
                    printf("hit newfd\n");
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
		                // got error or connection closed by client
		                if (nbytes == 0) {
			                // connection closed
			                printf("selectserver: socket %d hung up\n", i);
		                } else {
			                perror("recv");
		                }
		                close(i);
		                FD_CLR(i, &master); 
	                } else {
		                printf("Received %d bytes\n", nbytes);
		                printf("Message received was: %s\n", buf);
		                printf("time: sec: %ld m_sec: %ld", tv.tv_sec, tv.tv_usec);
		                tv.tv_sec = 10;
		                tv.tv_usec = 0;
		                memset(&buf, 0, sizeof buf);
	                }
                } else {
                    printf("didn't hit listener or newfd\n");
                }

            }
            
        }

    }

}

