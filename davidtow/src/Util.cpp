#include "Util.h"

#include <string>
#include <sstream>
#include <netinet/in.h>
#include <stdio.h>

std::string toString(int val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}


// get sockaddr, IPv4 or IPv6:
void* get_in_addr(struct sockaddr* sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void send(int fd, unsigned char* message, size_t size) {

	if (int out_bytes = ::send(fd, message, size, 0) == -1) {
		perror("send");
	} else {
		printf("SEND: response sent %d bytes successfully\n", out_bytes);
	}

}
