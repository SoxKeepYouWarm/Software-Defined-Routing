
#ifndef DATA_SOCKET_DEFINITIONS_H
#define DATA_SOCKET_DEFINITIONS_H


typedef struct Data_packet {
	unsigned char destination_router_ip[4];
	unsigned char transfer_id;
	unsigned char ttl;
	unsigned short sequence_number;
	int fin_and_padding;
	unsigned char data[1024];
} Data_packet;


#endif
