
#include <iostream>
#include <cstring>

#include "network_structures.h"

Control_message* Network_services::msg;
unsigned char* Network_services::buff;
unsigned char* Network_services::payload_pointer;
int Network_services::control_code;
int Network_services::payload_length;


void Network_services::encode_control_message_author_payload() {
	if (payload_length > 0) {
		memcpy(&msg->payload, payload_pointer, payload_length);
	}
}


void Network_services::encode_control_message_init_payload() {
	Control_message_init_payload payload;
	// each router entry is 12 bytes long

	unsigned short num_of_routers_bytes = 0;
	memcpy(&num_of_routers_bytes, payload_pointer, 2);
	num_of_routers_bytes = ntohs(num_of_routers_bytes);

	unsigned short update_period_bytes = 0;
	memcpy(&num_of_routers_bytes, payload_pointer + 2, 2);
	update_period_bytes = ntohs(update_period_bytes);

	payload.number_of_routers = num_of_routers_bytes;
	payload.update_period = update_period_bytes;

	std::cout << "HANDLE_INIT: payload_length: "
			<< msg->header.payload_length
			<< " num_of_routers " << num_of_routers_bytes << std::endl;


	payload.entry_list = new Init_payload_router_entry[num_of_routers_bytes];
	for (int i = 0; i < num_of_routers_bytes; i++) {

		int offset = (i * 12) + 4;
		Init_payload_router_entry entry;

		unsigned short id_bytes = 0;
		memcpy(&id_bytes, payload_pointer + offset, 2);
		id_bytes = ntohs(id_bytes);

		unsigned short port_one_bytes = 0;
		memcpy(&port_one_bytes, payload_pointer + offset + 2, 2);
		port_one_bytes = ntohs(port_one_bytes);

		unsigned short port_two_bytes = 0;
		memcpy(&port_two_bytes, payload_pointer + offset + 4, 2);
		port_two_bytes = ntohs(port_two_bytes);

		unsigned short cost_bytes = 0;
		memcpy(&cost_bytes, payload_pointer + offset + 6, 2);
		cost_bytes = ntohs(cost_bytes);

		uint32_t ip_bytes;
		memcpy(&ip_bytes, payload_pointer + offset + 8, 4);
		ip_bytes = ntohl(ip_bytes);

		entry.id = id_bytes;
		entry.port_one = port_one_bytes;
		entry.port_two = port_two_bytes;
		entry.cost = cost_bytes;

		memcpy(&entry.router_ip, &ip_bytes, 4);

		payload.entry_list[i] = entry;

	}

	msg->payload = &payload;
}


void Network_services::encode_control_message_routing_table_payload() {

}


void Network_services::encode_control_message_update_payload() {

	Control_message_update_payload payload;

	unsigned short router_id = 0;
	memcpy(&router_id, payload_pointer, 2);
	router_id = ntohs(router_id);

	unsigned short cost = 0;
	memcpy(&cost, payload_pointer + 2, 2);
	cost = ntohs(cost);

	payload.router_id = router_id;
	payload.cost = cost;

	msg->payload = &payload;

}


void Network_services::encode_control_message_crash_payload() {

}


void Network_services::encode_control_message_sendfile_payload() {
	Control_message_sendFile_payload payload;

	uint32_t ip_bytes = 0;
	memcpy(&ip_bytes, payload_pointer, 4);
	ip_bytes = ntohl(ip_bytes);
	memcpy(&payload, &ip_bytes, 4);

	payload.ttl = *(payload_pointer + 4);
	payload.transfer_id = *(payload_pointer + 5);

	unsigned short seq_num = 0;
	memcpy(&seq_num, payload_pointer + 6, 2);
	seq_num = ntohs(seq_num);
	memcpy(&payload + 6, &seq_num, 2);

	char filename[payload_length];
	memcpy(filename, payload_pointer, payload_length);
	payload.filename = filename;

	msg->payload = &payload;

}


void Network_services::encode_control_message_sendfile_stats_payload() {

}


void Network_services::encode_control_message_last_data_packet_payload() {

}


void Network_services::encode_control_message_penultimate_data_packet_payload() {

}


void Network_services::encode_control_message(Control_message* message, unsigned char* buffer) {

	msg = message;
	buff = buffer;
	control_code = message->header.control_code;
	payload_length = message->header.payload_length;
	payload_pointer = buffer + 8;

	memcpy(&message->header.destination_router_ip, buffer, 4);
	memcpy(&message->header.control_code, buffer + 4, 1);
	memcpy(&message->header.response_time, buffer + 5, 1);
	memcpy(&message->header.payload_length, buffer + 6, 2);

	switch (control_code) {
	case AUTHOR: {

		encode_control_message_author_payload();

	} break;
	case INIT: {

		encode_control_message_init_payload();

	} break;
	case ROUTING_TABLE: {

		encode_control_message_routing_table_payload();

	} break;
	case UPDATE: {

		encode_control_message_update_payload();

	} break;
	case CRASH: {

		encode_control_message_crash_payload();

	} break;
	case SENDFILE: {

		encode_control_message_sendfile_payload();

	} break;
	case SENDFILE_STATS: {

		encode_control_message_sendfile_stats_payload();

	} break;
	case LAST_DATA_PACKET: {

		encode_control_message_last_data_packet_payload();

	} break;
	case PENULTIMATE_DATA_PACKET: {

		encode_control_message_penultimate_data_packet_payload();

	} break;
	}

}
