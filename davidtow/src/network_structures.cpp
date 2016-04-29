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

Control_message* Network_services::msg;
unsigned char* Network_services::buff;
unsigned char* Network_services::payload_pointer;
unsigned char Network_services::control_code;
int Network_services::payload_length;


// get sockaddr, IPv4 or IPv6:
void* Network_services::get_in_addr(struct sockaddr* sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void Network_services::send(int fd, unsigned char* message, size_t size) {

	if (int out_bytes = ::send(fd, message, size, 0) == -1) {
		perror("send");
	} else {
		printf("SEND: response sent %d bytes successfully\n", out_bytes);
	}

}


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

		uint32_t ip_bytes;
		memcpy(&ip_bytes, payload_pointer + offset + 8, 4);
		ip_bytes = ntohl(ip_bytes);

		entry->id = id_bytes;
		entry->router_port = port_one_bytes;
		entry->data_port = port_two_bytes;
		entry->cost = cost_bytes;

		memcpy(&(entry->router_ip), &ip_bytes, 4);

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

	uint32_t ip_bytes = 0;
	memcpy(&ip_bytes, payload_pointer, 4);
	ip_bytes = ntohl(ip_bytes);
	payload->destination_router_ip = ip_bytes;
	//memcpy(&payload, &ip_bytes, 4);

	payload->ttl = *(payload_pointer + 4);
	payload->transfer_id = *(payload_pointer + 5);

	unsigned short seq_num = 0;
	memcpy(&seq_num, payload_pointer + 6, 2);
	seq_num = ntohs(seq_num);
	memcpy(&payload + 6, &seq_num, 2);

	char filename[payload_length];
	memcpy(filename, payload_pointer, payload_length);
	payload->filename = filename;

	msg->payload = (void*)payload;

}


void Network_services::encode_control_message_sendfile_stats_payload() {

	Control_message_sendFileStats_payload* payload = new Control_message_sendFileStats_payload;

	memcpy(&payload->transfer_id, payload_pointer, 1);

	msg->payload = (void*)payload;

}


void Network_services::encode_control_message(Control_message* message,
		unsigned char* buffer) {

	uint32_t dest_ip = 0;
	memcpy(&dest_ip, buffer, 4);
	dest_ip = ntohl(dest_ip);
	message->header.destination_router_ip = dest_ip;
	//memcpy(&message->header.destination_router_ip, buffer, 4);

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
		const std::vector< std::vector<Routing_table_entry> >* routing_table, int router_id,
		unsigned char* buffer) {

	uint32_t dest_ip = message->header.destination_router_ip;
	dest_ip = htonl(dest_ip);
	memcpy(buffer, &dest_ip, 4);

	memcpy(buffer + 4, &message->header.control_code, 1);

	message->header.response_time = 0;
	memcpy(buffer + 5, &message->header.response_time, 1);

	unsigned short payload_length = message->header.payload_length;
	payload_length = htons(payload_length);
	memcpy(buffer + 6, &payload_length, 2);

	for (int i = 1; i < routing_table->size(); i++) {
		const Routing_table_entry* entry = &routing_table->at(router_id).at(i);
		int buffer_offset = 8 + (i - 1) * 8;

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

	uint32_t dest_ip = message->header.destination_router_ip;
	dest_ip = htonl(dest_ip);
	memcpy(buffer, &dest_ip, 4);

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



}

