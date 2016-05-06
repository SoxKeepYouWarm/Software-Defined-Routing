
#ifndef DATA_SOCKET_DEFINITIONS_H
#define DATA_SOCKET_DEFINITIONS_H


typedef struct Data_packet {
	unsigned int destination_router_ip;
	unsigned char transfer_id;
	unsigned char ttl;
	unsigned short sequence_number;
	unsigned int fin_and_padding;
	unsigned char data[1024];
} Data_packet;


#endif
