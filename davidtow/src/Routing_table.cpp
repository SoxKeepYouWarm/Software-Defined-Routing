#include "Routing_table.h"

#include <iostream>
#include <string.h>

#include "Util.h"

Routing_table::Routing_table(Control_message_init_payload* init_payload) {

	routing_table_length = init_payload->number_of_routers;

	routing_table = new std::vector< std::vector<Routing_table_entry> >
	(routing_table_length + 1, std::vector<Routing_table_entry>(routing_table_length + 1));

	std::vector<Routing_table_entry> my_vector(routing_table_length + 1);

	for (int i = 0; i < routing_table_length; i++) {

		Init_payload_router_entry* entry = (init_payload->entry_list + i);

		Routing_table_entry new_entry;
		new_entry.id = entry->id;
		new_entry.cost = entry->cost;
		new_entry.next_hop = entry->id;

		my_vector.at(new_entry.id) = new_entry;

		if (entry->cost == 0) {
			this->my_router_id = entry->id;

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


	for (int i = 1; i < routing_table_length + 1; i++) {
		routing_table->at(this->my_router_id).at(i) = my_vector[i];
	}


	std::cout << "ROUTER_ID: " << my_router_id << std::endl;

		for (int i = 1; i < routing_table_length + 1; i++) {

			std::cout << "id: " << routing_table->at(my_router_id).at(i).id
				<< " cost: " << routing_table->at(my_router_id).at(i).cost
				<< " next_hop: " << routing_table->at(my_router_id).at(i).next_hop
				<< std::endl;

		}

}


Routing_table::~Routing_table() {

}


void Routing_table::update(int router_id, int cost) {

	this->routing_table->at(my_router_id).at(router_id).cost = cost;

}


const std::vector< std::vector<Routing_table_entry> >* Routing_table::get_routing_table() {
	return this->routing_table;
}













