#include "Routing_table.h"

#include <iostream>
#include <string.h>

#include "router.h"
#include "Timer.h"
#include "Util.h"


Routing_table::Routing_table(Router* router,
		Control_message_init_payload* init_payload): router(router) {

	this->logger = Logger::get_logger();

	routing_table_length = init_payload->number_of_routers;

	routing_table = new std::vector<Routing_table_vector>;

	for (int i = 0; i < routing_table_length; i++) {

		Init_payload_router_entry* entry = init_payload->entry_list + i;
		Routing_table_vector new_vector;

		new_vector.id = entry->id;
		memcpy(&new_vector.router_ip, &entry->router_ip, 4);
		new_vector.router_port = entry->router_port;
		new_vector.data_port = entry->data_port;
		new_vector.vector_entries = new std::vector<Routing_table_entry>;

		std::cout << "ROUTING_TABLE_ENTRY: id: " << entry->id
				<< " router_ip: " << entry->router_ip
				<< " router_port: " << entry->router_port
				<< " data_port: " << entry->data_port << std::endl;

		routing_table->push_back(new_vector);

		if (entry->cost == 0) {
			this->my_router_id = entry->id;
			this->get_vector(my_router_id)->vector_entries =
					new std::vector<Routing_table_entry>;

			const char* router_port = toString(entry->router_port).c_str();
			strcpy(this->my_router_port, router_port);
			const char* data_port = toString(entry->data_port).c_str();
			strcpy(this->my_data_port, data_port);

			std::cout << "ROUTING_TABLE: id: " << my_router_id
					<< " my_router_port: " << my_router_port
					<< " my_data_port: " << my_data_port << std::endl;

		}

	}

	if (this->my_router_id == -1) {
		std::cout << "ROUTING_TABLE: ERROR: "
				<< "couldn't find a routing entry id with cost 0"
				<< std::endl;
		exit(4);
	}

	logger->set_tag(::toString(my_router_id).c_str());
	logger->router_log("hello world\n");

	for (int i = 0; i < routing_table_length; i++) {

		Routing_table_vector* my_vector = this->get_vector(my_router_id);
		Init_payload_router_entry* entry = &init_payload->entry_list[i];

		Routing_table_entry new_entry;

		new_entry.id = entry->id;
		new_entry.cost = entry->cost;
		new_entry.next_hop = entry->id;

		my_vector->vector_entries->push_back(new_entry);

	}

	print_routing_table();

}


Routing_table::~Routing_table() {

	delete routing_table;

}


void Routing_table::update_cost(int router_id, int cost) {

	this->get_vector(my_router_id)->get_entry(router_id)->cost = cost;

}


void Routing_table::update_routing(Router_update_message* message) {

	unsigned short sender_router_id = -1;

	for (int i = 0; i < message->num_of_update_fields; i++) {

		if (message->update_entries[i].cost == 0) {
			sender_router_id = message->update_entries[i].id;
		}

	}

	if (sender_router_id == -1) {
		std::cout << "ERROR: UPDATE_ROUTING: "
				<< "sender_router_id was not set" << std::endl;
		return;
	} else {
		std::cout << "UPDATE_ROUTING: sender_router_id: "
				<< sender_router_id << std::endl;
	}

	Routing_table_vector* update_vector = this->get_vector(sender_router_id);

	for (int i = 0; i < message->num_of_update_fields; i++) {

		Router_update_entry* message_entry = &message->update_entries[i];
		int msg_id = message_entry->id;

		// checks if a vector table entry already exists
		if (Routing_table_entry* vector_entry = update_vector->get_entry(msg_id)) {

			// entry already exists
			vector_entry->cost = message_entry->cost;

			std::cout << "UPDATED_COST: "
					<< " router: " << sender_router_id
					<< " entry: " << msg_id
					<< " new_cost: " << vector_entry->cost << std::endl;

		} else {

			// entry does not exist
			Routing_table_entry new_entry;
			new_entry.id = message_entry->id;
			new_entry.cost = message_entry->cost;
			new_entry.next_hop = new_entry.id;

			update_vector->vector_entries->push_back(new_entry);

			std::cout << "UPDATE_ROUTING: "
					<< "created new entry for router_id: " << sender_router_id
					<< " entry_id: " << new_entry.id << std::endl;

		}

	}

	recalculate_vector(sender_router_id);

	router->timer->notify_routing_update_received(sender_router_id);

}


int Routing_table::distance(int router_id_src, int router_id_dest) {

	return this->get_vector(router_id_src)->get_entry(router_id_dest)->cost;

}


void Routing_table::recalculate_vector(int updated_router_id) {

	int distance_to_updated_router = distance(my_router_id, updated_router_id);

	for (std::vector<Routing_table_vector>::iterator id_iter = routing_table->begin();
			id_iter != routing_table->end(); id_iter++) {

		unsigned short target_id = (*id_iter).id;
		// skip my router_id
		if (target_id == my_router_id) continue;

		int new_cost = distance_to_updated_router + distance(updated_router_id, target_id);
		if (new_cost < distance(my_router_id, target_id)) {

			// shorter path found, update vector cost
			Routing_table_entry* my_entry = this->get_vector(my_router_id)->get_entry(target_id);
			my_entry->cost = new_cost;
			my_entry->next_hop = target_id;

			std::cout << "RECALCULATE_VECTOR: "
					<< "new cost to: " << target_id << " is: "
					<< new_cost << std::endl;

		}

	}

}


const std::vector<Routing_table_vector>* Routing_table::get_routing_table() {
	return this->routing_table;
}


Routing_table_vector* Routing_table::get_vector(int router_id) {

	for (std::vector<Routing_table_vector>::iterator iter = this->routing_table->begin();
			iter != this->routing_table->end(); iter++) {

		if ((*iter).id == router_id) return &(*iter);

	}

	return 0;

}


Routing_table_vector* Routing_table::get_my_vector() {
	return this->get_vector(my_router_id);
}


Routing_table_entry* Routing_table::get_my_vector_entry(int router_id) {
	return this->get_my_vector()->get_entry(router_id);
}


void Routing_table::print_routing_table() {

	std::cout << "ROUTER_ID: " << my_router_id << std::endl;

	for (int i = 0; i < routing_table_length; i++) {

		if (Routing_table_entry* entry = this->get_my_vector()->get_entry(i)) {
			std::cout << "id: " << entry->id
				<< " cost: " << entry->cost
				<< " next_hop: " << entry->next_hop
				<< std::endl;
		}

	}

}







