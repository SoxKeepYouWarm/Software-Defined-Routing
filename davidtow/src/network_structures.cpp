#include "network_structures.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <iostream>
#include <cstring>

#include "Control_socket_manager.h"
#include "Data_socket_manager.h"
#include "Router_socket_manager.h"
#include "Routing_table.h"


Control_message* Network_services::msg;
unsigned char* Network_services::buff;
unsigned char* Network_services::payload_pointer;
unsigned char Network_services::control_code;
int Network_services::payload_length;



void Network_services::encode_control_message_init_payload() {
	Control_message_init_payload* payload = new Control_message_init_payload;
	// each router entry is 12 bytes long

	unsigned short num_of_routers_bytes = 0;
	memcpy(&num_of_routers_bytes, payload_pointer, 2);
	num_of_routers_bytes = ntohs(num_of_routers_bytes);

	unsigned short update_period_bytes = 0;
	memcpy(&update_period_bytes, payload_pointer + 2, 2);
	update_period_bytes = ntohs(update_period_bytes);

	payload->number_of_routers = num_of_routers_bytes;
	payload->update_period = update_period_bytes;

	printf("ENCODE_INIT: payload length: %d, num_of_routers: %d\n",
			msg->header.payload_length,
			num_of_routers_bytes);

	payload->entry_list = new Init_payload_router_entry[num_of_routers_bytes];
	for (int i = 0; i < num_of_routers_bytes; i++) {

		int offset = (i * 12) + 4;
		Init_payload_router_entry* entry = &(payload->entry_list[i]);

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

		memcpy(&entry->router_ip, payload_pointer + offset + 8, 4);

		entry->id = id_bytes;
		entry->router_port = port_one_bytes;
		entry->data_port = port_two_bytes;
		entry->cost = cost_bytes;

	}

	msg->payload = (void*)payload;
}


void Network_services::encode_control_message_update_payload() {

	Control_message_update_payload* payload = new Control_message_update_payload;

	unsigned short router_id = 0;
	memcpy(&router_id, payload_pointer, 2);
	router_id = ntohs(router_id);

	unsigned short cost = 0;
	memcpy(&cost, payload_pointer + 2, 2);
	cost = ntohs(cost);

	payload->router_id = router_id;
	payload->cost = cost;

	msg->payload = (void*)payload;

}


void Network_services::encode_control_message_sendfile_payload() {

	Control_message_sendFile_payload* payload = new Control_message_sendFile_payload;

	memcpy(&payload->destination_router_ip, payload_pointer, 4);

	payload->ttl = *(payload_pointer + 4);
	payload->transfer_id = *(payload_pointer + 5);

	unsigned short seq_num = 0;
	memcpy(&seq_num, payload_pointer + 6, 2);
	seq_num = ntohs(seq_num);
	memcpy(&payload + 6, &seq_num, 2);

	payload->filename = new char[payload_length];
	memcpy(payload->filename, payload_pointer, payload_length);

	msg->payload = (void*)payload;

}


void Network_services::encode_control_message_sendfile_stats_payload() {

	Control_message_sendFileStats_payload* payload = new Control_message_sendFileStats_payload;

	memcpy(&payload->transfer_id, payload_pointer, 1);

	msg->payload = (void*)payload;

}


void Network_services::encode_control_message(Control_message* message,
		unsigned char* buffer) {

	memcpy(&message->header.destination_router_ip, buffer, 4);

	memcpy(&message->header.control_code, buffer + 4, 1);
	memcpy(&message->header.response_time, buffer + 5, 1);

	unsigned short payload_length = 0;
	memcpy(&payload_length, buffer + 6, 2);
	payload_length = ntohs(payload_length);
	message->header.payload_length = payload_length;

	msg = message;
	buff = buffer;
	control_code = message->header.control_code;
	payload_length = message->header.payload_length;
	payload_pointer = buffer + 8;

	switch (control_code) {
	case INIT:
		encode_control_message_init_payload();
		printf("DEBUG: num_of_routers: %d\n", (
				(Control_message_init_payload*)msg->payload)->number_of_routers);
		break;
	case UPDATE:
		encode_control_message_update_payload();
		break;
	case SENDFILE:
		encode_control_message_sendfile_payload();
		break;
	case SENDFILE_STATS:
		encode_control_message_sendfile_stats_payload();
		break;
	}

}


void Network_services::decode_control_message_author() {

	if (payload_length > 0) {
		memcpy(buff + 8, msg->payload, payload_length);
	}

}


void Network_services::decode_control_message_routing_table(Control_message* message,
		Routing_table* routing_table, int router_id,
		unsigned char* buffer) {

	memcpy(buffer, &message->header.destination_router_ip, 4);

	memcpy(buffer + 4, &message->header.control_code, 1);

	message->header.response_time = 0;
	memcpy(buffer + 5, &message->header.response_time, 1);

	unsigned short payload_length = message->header.payload_length;
	payload_length = htons(payload_length);
	memcpy(buffer + 6, &payload_length, 2);

	for (int i = 0; i < routing_table->routing_table_length; i++) {
		const Routing_table_entry* entry =
				&routing_table->get_vector(router_id)->vector_entries->at(i);

		int buffer_offset = 8 + i * 8;

		unsigned short id = entry->id;
		id = htons(id);
		memcpy(buffer + buffer_offset, &id, 2);

		// used as two bytes of padding
		memset(buffer + buffer_offset + 2, 0, 2);

		unsigned short next_hop = entry->next_hop;
		next_hop = htons(next_hop);
		memcpy(buffer + buffer_offset + 4, &next_hop, 2);

		unsigned short cost = entry->cost;
		cost = htons(cost);
		memcpy(buffer + buffer_offset + 6, &cost, 2);

	}

}


void Network_services::decode_control_message(Control_message* message,
		unsigned char* buffer) {

	memcpy(buffer, &message->header.destination_router_ip, 4);

	msg = message;
	buff = buffer;
	payload_pointer = buffer + 8;
	control_code = message->header.control_code;
	payload_length = message->header.payload_length;

	memcpy(buffer + 4, &message->header.control_code, 1);

	message->header.response_time = 0;
	memcpy(buffer + 5, &message->header.response_time, 1);

	unsigned short payload_length = message->header.payload_length;
	payload_length = htons(payload_length);
	memcpy(buffer + 6, &payload_length, 2);


	switch (control_code) {

	case AUTHOR:
		decode_control_message_author();
		break;

	}


}


void Network_services::encode_data_message(Data_packet* message,
		unsigned char* buffer) {

	memcpy(&message->destination_router_ip, buffer, 4);
	memcpy(&message->transfer_id, buffer + 4, 1);
	memcpy(&message->ttl, buffer + 5, 1);

	unsigned short seq_num = 0;
	memcpy(&seq_num, buffer + 6, 2);
	seq_num = ntohs(seq_num);
	message->sequence_number = seq_num;

	unsigned int fin = 0;
	memcpy(&fin, buffer + 8, 4);
	fin = ntohl(fin);
	message->fin_and_padding = fin;

	memcpy(&message->data, buffer + 12, 1024);

}


void Network_services::decode_data_message(Data_packet* message,
		unsigned char* buffer) {

	memcpy(buffer, &message->destination_router_ip, 4);
	memcpy(buffer + 4, &message->transfer_id, 1);
	memcpy(buffer + 5, &message->ttl, 1);

	unsigned short seq_num = message->sequence_number;
	seq_num = htons(seq_num);
	memcpy(buffer + 6, &seq_num, 2);

	unsigned int fin = message->fin_and_padding;
	fin = htonl(fin);
	memcpy(buffer + 8, &fin, 4);

	memcpy(buffer + 12, &message->data, 1024);

}


void Network_services::encode_router_message(Router_update_message* message,
		unsigned char* buffer) {

	unsigned short num_of_update_fields = 0;
	memcpy(&num_of_update_fields, buffer, 2);
	num_of_update_fields = ntohs(num_of_update_fields);
	message->num_of_update_fields = num_of_update_fields;

	unsigned short source_router_port = 0;
	memcpy(&source_router_port, buffer + 2, 2);
	source_router_port = ntohs(source_router_port);
	message->router_port = source_router_port;

	memcpy(&message->router_ip, buffer + 4, 4);

	message->update_entries = new Router_update_entry[message->num_of_update_fields];

	for (int i = 0; i < message->num_of_update_fields; i++) {

		Router_update_entry* entry = &message->update_entries[i];
		int buffer_offset = 8 + (i * 12);

		memcpy(&entry->router_ip, buffer + buffer_offset, 4);

		unsigned short port = 0;
		memcpy(&port, buffer + buffer_offset + 4, 2);
		port = ntohs(port);
		memcpy(&entry->router_port, &port, 2);

		unsigned short id = 0;
		memcpy(&id, buffer + buffer_offset + 8, 2);
		id = ntohs(id);
		memcpy(&entry->id, &id, 2);

		unsigned short cost = 0;
		memcpy(&cost, buffer + buffer_offset + 10, 2);
		cost = ntohs(cost);
		memcpy(&entry->cost, &cost, 2);

	}

}


void Network_services::decode_routing_table_vector(Routing_table* routing_table,
		unsigned char* buffer) {

	Routing_table_vector* my_vector = routing_table->get_shortest_paths();

	unsigned short routing_table_size = my_vector->vector_entries->size();
	routing_table_size = htons(routing_table_size);
	memcpy(buffer, &routing_table_size, 2);

	unsigned short src_router_port = my_vector->router_port;
	src_router_port = htons(src_router_port);
	memcpy(buffer + 2, &src_router_port, 2);

	memcpy(buffer + 4, &my_vector->router_ip, 4);

	for (int i = 0; i < my_vector->vector_entries->size(); i++) {

		Routing_table_entry* entry = &my_vector->vector_entries->at(i);
		Routing_table_vector* entry_vector = routing_table->get_vector(entry->id);

		int buffer_offset = 8 + (i * 12);

		memcpy(buffer + buffer_offset, &entry_vector->router_ip, 4);

		unsigned short router_port = entry_vector->router_port;
		router_port = htons(router_port);
		memcpy(buffer + buffer_offset + 4, & router_port, 2);

		memset(buffer + buffer_offset + 6, 0, 2);

		unsigned short router_id = entry_vector->id;
		router_id = htons(router_id);
		memcpy(buffer + buffer_offset + 8, &router_id, 2);

		unsigned short router_cost = entry->cost;
		router_cost = htons(router_cost);
		memcpy(buffer + buffer_offset + 10, &router_cost, 2);

	}

}








