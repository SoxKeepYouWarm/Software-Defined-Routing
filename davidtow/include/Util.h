
#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <fstream>

std::string toString(int val);

void* get_in_addr(struct sockaddr* sa);

void send(int fd, unsigned char* message, size_t size);

class Logger {

	static Logger* logger;
	char tag[16];
	char router_log_filename[32];
	char data_log_filename[32];
	std::ofstream router_log_file;
	std::ofstream data_log_file;
	int tag_set;

	Logger();
	~Logger();

public:
	static Logger* get_logger();
	void set_tag(char* tag);
	void set_tag(const char* tag);

	void router_log(char* msg);
	void router_log(const char* format, ...);

	void data_log(char* msg);
	void data_log(const char* format, ...);

};

#endif
