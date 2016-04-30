#include "Routing_table.h"

#include <iostream>
#include <string.h>

#include "router.h"
#include "Timer.h"
#include "Util.h"


Routing_table::Routing_table(Router* router,
		Control_message_init_payload* init_payload): router(router) {

	routing_table_length = init_payload->number_of_routers;

	routing_table = new std::vector<Routing_table_vector>;

	for (int i = 0; i < routing_table_length; i++) {

		Init_payload_router_entry* entry = &init_payload->entry_list[i];
		Routing_table_vector new_vector;

		new_vector.id = entry->id;
		memcpy(new_vector.router_ip, entry->router_ip, 4);
		new_vector.router_port = entry->router_port;
		new_vector.data_port = entry->data_port;

		routing_table->push_back(new_vector);

		if (entry->cost == 0) {
			this->my_router_id = entry->id;
			this->get_vector(my_router_id)->vector_entries =
					new std::vector<Routing_table_entry>;

			const char* router_port = toString(entry->router_port).c_str();
			const char* data_port = toString(entry->data_port).c_str();

			strcpy(this->my_router_port, router_port);
			strcpy(this->my_data_port, data_port);

		}

	}

	if (this->my_router_id == -1) {
		std::cout << "ROUTING_TABLE: ERROR: "
				<< "couldn't find a routing entry id with cost 0"
				<< std::endl;
		exit(4);
	}

	for (int i = 0; i < routing_table_length; i++) {

		Routing_table_vector* my_vector = this->get_vector(my_router_id);
		Init_payload_router_entry* entry = &init_payload->entry_list[i];

		Routing_table_entry new_entry;

		new_entry.id = entry->id;
		new_entry.cost = entry->cost;

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

	// update routing table

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
	}

	Routing_table_vector* update_vector = this->get_vector(sender_router_id);

	for (int i = 0; i < message->num_of_update_fields; i++) {

		Router_update_entry* message_entry = &message->update_entries[i];

		// checks if a vector table entry already exists
		if (Routing_table_entry* vector_entry = update_vector->get_entry(message_entry->id)) {

			// entry already exists
			vector_entry->cost = message_entry->cost;

			// TODO calculate new first hop

		} else {

			// entry does not exist
			Routing_table_entry new_entry;
			new_entry.id = message_entry->id;
			new_entry.cost = message_entry->cost;

			// TODO calculate first hop

			update_vector->vector_entries->push_back(new_entry);

		}

	}

	router->timer->notify_routing_update_received(sender_router_id);

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


void Routing_table::print_routing_table() {

	std::cout << "ROUTER_ID: " << my_router_id << std::endl;

	for (int i = 0; i < routing_table_length; i++) {

		std::cout << "id: " << this->get_vector(my_router_id)->get_entry(i)->id
				<< " cost: " << this->get_vector(my_router_id)->get_entry(i)->cost
				<< " next_hop: " << this->get_vector(my_router_id)->get_entry(i)->next_hop
				<< std::endl;

	}

}







