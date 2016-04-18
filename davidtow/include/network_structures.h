
#ifndef NETWORK_STRUCTURES_H
#define NETWORK_STRUCTURES_H

#define AUTHOR 						0
#define INIT 						1
#define ROUTING_TABLE 				2
#define UPDATE 						3
#define CRASH 						4
#define SENDFILE 					5
#define SENDFILE_STATS 				6
#define LAST_DATA_PACKET 			7
#define PENULTIMATE_DATA_PACKET		8

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef struct Router_update_entry {
	char router_ip[32];
	unsigned short port;
	unsigned short padding;
	unsigned short id;
	unsigned short cost;

} Router_update_entry;


typedef struct Router_update_packet {
	unsigned short number_of_fields;
	unsigned short source_router_port;
	unsigned char source_router_ip[4];
	Router_update_entry* entry_list;
} Router_update_packet;


typedef struct Control_message_header {
	unsigned char destination_router_ip[4];
	unsigned char control_code;
	unsigned char response_time;
	unsigned short payload_length;
} Control_message_header;

typedef struct Control_message {
	Control_message_header header;
	void* payload;
} Control_message;

// UPDATE PERIOD: seconds between successive routing update
// packet broadcast
// PORT_ONE: router port
// PORT_TWO: data port
typedef struct Init_payload_router_entry {
	unsigned short id;
	unsigned short port_one;
	unsigned short port_two;
	unsigned short cost;
	unsigned char router_ip[4];
} Init_payload_router_entry;


typedef struct Control_message_init_payload {
	unsigned short number_of_routers;
	unsigned short update_period;
	Init_payload_router_entry* entry_list;
} Control_message_init_payload;


typedef struct Routing_table_entry {
	unsigned short id;
	unsigned short padding;
	unsigned short next_hop_id;
	unsigned short cost;
} Routing_table_entry;

typedef struct Control_response_routingTable_payload {
	Routing_table_entry* routing_table;
} Control_response_routingTable_payload;


typedef struct Control_message_update_payload {
	unsigned short router_id;
	unsigned short cost;
} Control_message_update_payload;


typedef struct Control_message_sendFile_payload {
	unsigned char destination_router_ip[4];
	unsigned char ttl;
	unsigned char transfer_id;
	unsigned short init_sequence_number;
	char* filename;
} Control_message_sendFile_payload;


typedef struct Control_message_sendFileStats_payload {
	unsigned char transfer_id;
} Control_message_sendFileStats_payload;


typedef struct Control_response_sendFileStats_payload {
	unsigned char transfer_id;
	unsigned char ttl;
	unsigned short padding;
	unsigned short* packet_sequence_number_list;
} Control_response_sendFileStats_payload;


typedef struct Data_packet {
	unsigned char destination_router_ip[4];
	unsigned char transfer_id;
	unsigned char ttl;
	unsigned short sequence_number;
	int fin_and_padding;
	unsigned char data[1024];
} Data_packet;


class Network_services {

	static Control_message* msg;
	static unsigned char* buff;
	static unsigned char* payload_pointer;
	static unsigned char control_code;
	static int payload_length;

	static void encode_control_message_init_payload();
	static void encode_control_message_update_payload();
	static void encode_control_message_sendfile_payload();
	static void encode_control_message_sendfile_stats_payload();

	static void decode_control_message_author();

public:
	static void encode_control_message(Control_message* message,
			unsigned char* buffer);
	static void decode_control_message(Control_message* message,
			unsigned char* buffer);

	static void encode_data_message(Data_packet* message,
			unsigned char* buffer);

	static void* get_in_addr(struct sockaddr* sa);
	static void send(int fd, unsigned char* message, size_t size);
};


#endif
