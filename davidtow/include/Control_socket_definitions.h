
#ifndef CONTROL_SOCKET_DEFINITIONS_H
#define CONTROL_SOCKET_DEFINITIONS_H

#include <stdint.h>

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
	unsigned short router_port;
	unsigned short data_port;
	unsigned short cost;
	unsigned char router_ip[4];
} Init_payload_router_entry;


typedef struct Control_message_init_payload {
	unsigned short number_of_routers;
	unsigned short update_period;
	Init_payload_router_entry* entry_list;
} Control_message_init_payload;


typedef struct Control_response_routingTable_entry {
	unsigned short id;
	unsigned short padding;
	unsigned short next_hop_id;
	unsigned short cost;
} Control_response_routingTable_entry;


typedef struct Control_response_routingTable_payload {
	Control_response_routingTable_entry* routing_table;
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


#endif
