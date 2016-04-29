
#ifndef UTIL_H_
#define _UTIL_H_

#include <string>

std::string toString(int val);

void* get_in_addr(struct sockaddr* sa);

void send(int fd, unsigned char* message, size_t size);

#endif
