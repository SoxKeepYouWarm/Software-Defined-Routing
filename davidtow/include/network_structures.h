
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

#define INFINITY 					0xFFFF

#include <stdlib.h>
#include <vector>

#include "Control_socket_definitions.h"
#include "Data_socket_definitions.h"
#include "Router_socket_definitions.h"


class Routing_table;

struct Control_message;
struct Control_message_init_payload;
struct Data_packet;
struct Router_update_message;
struct Routing_table_entry;

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

	static void decode_control_message_routing_table(
			Control_message* message,
			Routing_table* routing_table,
			int router_id,
			unsigned char* buffer);

	static void encode_data_message(Data_packet* message,
			unsigned char* buffer);

	static void encode_router_message(Router_update_message* message,
			unsigned char* buffer);

};


#endif
