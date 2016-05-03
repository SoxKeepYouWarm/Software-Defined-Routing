#include "Util.h"

#include <string>
#include <string.h>
#include <sstream>
#include <netinet/in.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdarg.h>


std::string toString(int val)
{
    std::stringstream stream;
    stream.clear();
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


Logger* Logger::logger = 0;

Logger::Logger() {
	tag_set = 0;

}


Logger::~Logger() {
	router_log_file.close();
	delete logger;
}


Logger* Logger::get_logger() {
	if ( ! logger ) logger = new Logger();
	return logger;
}


void Logger::set_tag(char* tag) {
	strcpy(this->tag, tag);
	tag_set = 1;
	strcpy(router_log_filename, "router_log");
	strcat(router_log_filename, tag);
	strcat(router_log_filename, ".log");
}


void Logger::set_tag(const char* tag) {
	strcpy(this->tag, tag);
	tag_set = 1;
	strcpy(router_log_filename, "router_log");
	strcat(router_log_filename, tag);
	strcat(router_log_filename, ".log");
	router_log_file.open (router_log_filename, std::ios::out);
	router_log_file.close();
}


void Logger::router_log(char* msg) {
	if (! tag_set) return;
	router_log_file.open (router_log_filename, std::ios::out | std::ios::app);
	if (router_log_file.is_open()) {
		router_log_file << msg;
		std::cout << msg;
	} else {
		std::cout << "error writing to log" << std::endl;
	}

	router_log_file.close();

}


void Logger::router_log(const char * format, ... ) {
	if (! tag_set) return;
	router_log_file.open (router_log_filename, std::ios::out | std::ios::app);
	if (router_log_file.is_open()) {
		char buffer[256];
		va_list args;
		va_start (args, format);
		vsprintf (buffer,format, args);
		router_log_file << buffer;
		std::cout << buffer;
		va_end (args);
	} else {
		std::cout << "error writing to log" << std::endl;
	}

	router_log_file.close();

}


