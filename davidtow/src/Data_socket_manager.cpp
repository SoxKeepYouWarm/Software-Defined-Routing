#include "Data_socket_manager.h"

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "router.h"
#include "Routing_table.h"
#include "Util.h"


Data_socket_manager::Data_socket_manager(Router* router,
		const char* port) {
	this->logger = Logger::get_logger();
	this->router = router;
	this->port = new char[6];
	strcpy(this->port, port);
	this->listener = 0;
	this->request_fd = 0;
	this->new_fd = 0;
	this->addrlen = 0;
	this->res = 0;
	this->p = 0;
	this->MAXIMUM_CONNECTIONS = 1;

	memset(this->incoming_buffer, 0, sizeof incoming_buffer);
	this->incoming_res = 0;
	this->incoming_p = 0;
	this->incoming_socket = 0;

	memset(this->outgoing_buffer, 0, sizeof outgoing_buffer);
	this->outgoing_res = 0;
	this->outgoing_p = 0;
	this->outgoing_socket = 0;

	memset(this->last_packet, 0, sizeof last_packet);
	memset(this->second_last_packet, 0, sizeof second_last_packet);


	std::cout << "DATA_SOCKET_MANAGER: initialized with port: "
			<< port << std::endl;
}


void Data_socket_manager::initialize_addrinfo() {

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int rv;
	if ((rv = getaddrinfo(NULL, port, &hints, &res)) != 0) {
		fprintf(stderr, "DATA_SOCKET_MANAGER: %s\n", gai_strerror(rv));
		exit(1);
	}

}


void Data_socket_manager::handle_packet(Data_packet* packet) {

	packet->ttl = (unsigned char)((int)packet->ttl - 1);

	if (packet->destination_router_ip ==
			router->routing_table->get_my_vector()->router_ip) {
		// file is destined for this router
		logger->data_log("HANDLE_DATA: packet is destined for this router\n");
		write_data_to_file(packet);
	} else {
		// file needs to be forwarded

		if ((int)packet->ttl > 0) {
			logger->data_log("HANDLE_DATA: packet is being forwarded\n");

			// check if an outbound connection has already been established
			if (! outgoing_socket) {
				logger->data_log("HANDLE_DATA: connection being initialized\n");
				initialize_connection(packet->destination_router_ip);
			}

			send_data(packet);

			if (packet->fin_and_padding == 0x8000) {
				close_connection();
			}

		} else {
			// ttl is 0, drop packet
			logger->data_log("HANDLE_DATA: ttl is 0, dropping packet\n");
		}
	}

}


Data_record* Data_socket_manager::get_data_record(unsigned char transfer_id) {

	for (std::vector<Data_record>::iterator iter = transfer_records.begin();
			iter != transfer_records.end(); iter++) {
		if (transfer_id == (*iter).transfer_id) return &(*iter);
	}

	return 0;

}


void Data_socket_manager::update_transfer_records(Data_packet* packet) {

	if (Data_record* existing_record = get_data_record(packet->transfer_id)) {
		existing_record->seq_nums.push_back(packet->sequence_number);
		logger->data_log("UPDATE_TRANSFER_LOG: updating record id: %d\n",
				packet->transfer_id);
	} else {
		Data_record new_record;
		new_record.transfer_id = packet->transfer_id;
		new_record.ttl = packet->ttl;
		new_record.seq_nums.push_back(packet->sequence_number);
		transfer_records.push_back(new_record);
		logger->data_log("UPDATE_TRANSFER_LOG: adding new record id: %d\n",
				packet->transfer_id);
	}

}


void Data_socket_manager::handle_data() {

	logger->data_log("HANDLE_DATA: handle data hit\n");

	if ((num_of_bytes = recv(incoming_socket, incoming_buffer, sizeof incoming_buffer, 0)) <= 0) {
		// got error or connection closed by client
		if (num_of_bytes == 0) {
			// connection closed
			logger->data_log("HANDLE_DATA: connection hung up\n");
			std::cout << "MESSAGE CAN BE COMBINED HERE" << std::endl;

		} else {
			perror("recv");
		}
		close(incoming_socket);
		router->unregister_fd(incoming_socket);
		connections.erase(incoming_socket);
	} else {

		logger->data_log("HANDLE_DATA: received: %d bytes\n", num_of_bytes);

		for (int i = 0; i < num_of_bytes; i++) {
			std::cout << std::hex << (int)incoming_buffer[i] << " ";
		}
		std::cout << std::dec << std::endl;

		// handle incoming data
		Data_packet message;
		Network_services::encode_data_message(&message, incoming_buffer);

		// update last packet and penultimate packet record
		memcpy(second_last_packet, last_packet, 1024);
		memcpy(last_packet, message.data + 12, 1024);

		logger->data_log("HANDLE_DATA: destination_router_ip: %u | transfer_id: %u | "
				"ttl: %u | seq_num: %u | fin: %u\n", message.destination_router_ip,
				message.transfer_id, message.ttl, message.sequence_number,
				message.fin_and_padding);

		handle_packet(&message);

		// update transfer records
		update_transfer_records(&message);

		memset(&outgoing_buffer, 0, sizeof outgoing_buffer);

	}

}


void Data_socket_manager::handle_listener() {

	logger->data_log("DATA_SOCKET_MANAGER: handle listener hit\n");

	addrlen = sizeof remoteaddr;
	incoming_socket = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
	if (incoming_socket == -1) {
		std::cout << "ERROR: accepting incoming socket" << std::endl;
		perror("accept");
		exit(1);
	}

	logger->data_log("DATA_SOCKET_MANAGER: socket accepted\n");

	connections.insert(incoming_socket);
	router->register_fd(incoming_socket);

}


void Data_socket_manager::handle_connection(int fd) {
	std::cout << "DATA_SOCKET_MANAGER: handle connection hit" << std::endl;

	//request_fd = fd;
	if (fd == listener) {
		handle_listener();
	} else {
		handle_data();
	}

}


void Data_socket_manager::initialize_connection(unsigned int ip) {

	unsigned int destination_router_id =
			router->routing_table->get_routerId_from_ip(ip);

	unsigned int next_hop_id =
			router->routing_table->get_next_hop_routerId(destination_router_id);

	unsigned int next_hop_router_ip =
			router->routing_table->get_vector(next_hop_id)->router_ip;
	unsigned int next_hop_router_port =
			router->routing_table->get_vector(next_hop_id)->data_port;

	memset(&outgoing_hints, 0, sizeof outgoing_hints);
	outgoing_hints.ai_family = AF_UNSPEC;
	outgoing_hints.ai_socktype = SOCK_STREAM;
	outgoing_hints.ai_flags = AI_PASSIVE;

	char ip_str[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &next_hop_router_ip,
			ip_str, INET_ADDRSTRLEN);

	char port_str[6];
	strcpy(port_str, ::toString(next_hop_router_port).c_str());

	int rv;
	if ((rv = getaddrinfo(ip_str, port_str, &outgoing_hints, &outgoing_res)) != 0) {
		logger->data_log("DATA_SOCKET: send data error: %s\n", gai_strerror(rv));
		fprintf(stderr, "data_socket:send_data: %s\n", gai_strerror(rv));
		exit(1);
	}


	for(outgoing_p = outgoing_res; outgoing_p != NULL; outgoing_p = outgoing_p->ai_next) {
		outgoing_socket = socket(outgoing_p->ai_family, outgoing_p->ai_socktype, outgoing_p->ai_protocol);
		if (outgoing_socket < 0) {
			continue;
		}

		int yes = 1;
		setsockopt(outgoing_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		break;
	}

	// if we got here, it means we didn't get bound
	if (outgoing_p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	if (connect(outgoing_socket, outgoing_res->ai_addr, outgoing_res->ai_addrlen) < 0){
		logger->data_log("DATA_SOCKET_MANAGER: send_data: error on connect\n");
		std::cerr << "DATA_SOCKET_MANAGER: error on connect" << std::endl;
	} else {
		logger->data_log("DATA_SOCKET_MANAGER: send_data: connected send socket\n");
	}

}


void Data_socket_manager::send_data(Data_packet* data) {

	// send data to target

	Network_services::decode_data_message(data, outgoing_buffer);

	int sent_bytes = send(outgoing_socket, outgoing_buffer, sizeof outgoing_buffer, 0);
	//int ack_bytes = recv(send_socket, ack_buffer, ack_size, 0);
	logger->data_log("SEND_DATA: sent %d bytes\n", sent_bytes);

}


void Data_socket_manager::close_connection() {
	freeaddrinfo(outgoing_res);
	close(outgoing_socket);
	outgoing_socket = 0;
}


void Data_socket_manager::write_data_to_file(Data_packet* data) {

	logger->data_log("WRITE_DATA_TO_FILE: writing data to file\n");

	char filename[64];
	strcpy(filename, "file-");
	strcat(filename, ::toString((int)data->transfer_id).c_str());

	logger->data_log("WRITE_DATA_TO_FILE: filename is: %s\n", filename);

	std::ofstream output;
	output.open(filename, std::ofstream::app | std::ofstream::binary);

	if (output.is_open()) {
		output.write((char*)(&data->data[0]), sizeof data->data);
	} else {
		logger->data_log("WRITE_DATA_TO_FILE: failed to open output file\n");
	}

}


const unsigned char* Data_socket_manager::get_last_packet() {
	return this->last_packet;
}


const unsigned char* Data_socket_manager::get_second_last_packet() {
	return this->second_last_packet;
}









