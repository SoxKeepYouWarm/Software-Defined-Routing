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

	shortest_paths.vector_entries = new std::vector<Routing_table_entry>;

	routing_table = new std::vector<Routing_table_vector>;

	// initialize routing table vectors, find my router_id
	for (int i = 0; i < routing_table_length; i++) {

		Init_payload_router_entry* entry = init_payload->entry_list + i;
		Routing_table_vector new_vector;

		new_vector.id = entry->id;
		memcpy(&new_vector.router_ip, &entry->router_ip, 4);
		new_vector.router_port = entry->router_port;
		new_vector.data_port = entry->data_port;
		new_vector.vector_entries = new std::vector<Routing_table_entry>;

		logger->router_log("ROUTING_TABLE_ENTRY: id: %d | "
				"router_ip: %d | router_port: %d | data_port: %d\n",
				entry->id, entry->router_ip, entry->router_port, entry->data_port);

		routing_table->push_back(new_vector);

		if (entry->cost == 0) {
			this->my_router_id = entry->id;
			//this->get_vector(my_router_id)->vector_entries =
			//		new std::vector<Routing_table_entry>;
			//std::cout << "ROUTER COST IS 0 PORT: " << entry->router_port << std::endl;
			const char* router_port = toString(entry->router_port).c_str();
			//std::cout << "ROUTER PORT AS STRING: " << router_port << std::endl;
			strcpy(this->my_router_port, router_port);
			const char* data_port = toString(entry->data_port).c_str();
			strcpy(this->my_data_port, data_port);

		}

	}

	if (this->my_router_id == -1) {
		std::cout << "ROUTING_TABLE: ERROR: "
				<< "couldn't find a routing entry id with cost 0"
				<< std::endl;
		exit(4);
	}

	logger->set_tag(::toString(my_router_id).c_str());
	logger->router_log("ROUTING_TABLE: id: %d | "
			"my_router_port: %s | my_data_port: %s\n",
			my_router_id, my_router_port, my_data_port);
	//std::cout << "router port: " << my_router_port << " data_port: " << my_data_port << std::endl;
	// initialize my routing vector entries
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
			if (vector_entry->cost != message_entry->cost) {
				logger->router_log("UPDATED_COST: "
						"router_id: %d | entry: %d | "
						"old_cost: %d | new_cost: %d\n",
						sender_router_id, msg_id,
						vector_entry->cost, message_entry->cost);
				vector_entry->cost = message_entry->cost;
			} else {
				logger->router_log("UPDATED_COST: "
						"SAME_COST: router_id: %d | "
						"entry: %d | cost: %d\n",
						sender_router_id, msg_id, vector_entry->cost);
			}

		} else {

			if (i == 0) {
				logger->router_log("UPDATED_ROUTING: "
						"adding new vector entries for router_id: %d\n",
						message_entry->id);
			}

			// entry does not exist
			Routing_table_entry new_entry;
			new_entry.id = message_entry->id;
			new_entry.cost = message_entry->cost;
			new_entry.next_hop = new_entry.id;

			update_vector->vector_entries->push_back(new_entry);

		}

	}

	recalculate_vector(sender_router_id);

	router->timer->notify_routing_update_received(sender_router_id);

}


int Routing_table::distance(int router_id_src, int router_id_dest) {

	return this->get_vector(router_id_src)->get_entry(router_id_dest)->cost;

}


void Routing_table::recalculate_vector(int updated_router_id) {

	for (std::vector<Routing_table_vector>::iterator vector_iter = routing_table->begin();
			vector_iter != routing_table->end(); vector_iter++) {

		int current_dest_router_id = (*vector_iter).id;

		int proposed_cost = INFINITY;
		int next_hop_router;

		for (std::vector<Routing_table_vector>::iterator search_iter = routing_table->begin();
				search_iter != routing_table->end(); search_iter++) {

			int search_cost;
			int proposed_next_hop_router;

			if ((*search_iter).id == my_router_id) {
				search_cost = distance(my_router_id, current_dest_router_id);
				proposed_next_hop_router = current_dest_router_id;
			} else {

				// skip routers I haven't received an vector from yet
				if (!(*search_iter).get_entry(current_dest_router_id)) continue;

				search_cost = distance(my_router_id, (*search_iter).id)
						+ distance((*search_iter).id, current_dest_router_id);
				proposed_next_hop_router = (*search_iter).id;
			}

			if (search_cost < proposed_cost) {
				proposed_cost = search_cost;
				next_hop_router = proposed_next_hop_router;
			}

		}

		// get shortset_path entry for current router
		if (Routing_table_entry* shortest_entry =
				shortest_paths.get_entry(current_dest_router_id)) {
			shortest_entry->cost = proposed_cost;
			shortest_entry->next_hop = next_hop_router;
		} else {
			Routing_table_entry new_entry;
			new_entry.id = current_dest_router_id;
			new_entry.cost = proposed_cost;
			new_entry.next_hop = next_hop_router;
			shortest_paths.vector_entries->push_back(new_entry);
		}


	}

	print_shortest_paths();

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

	logger->router_log("ROUTER_TABLE: ROUTER_ID: %d\n", my_router_id);

	for (int i = 0; i < routing_table_length; i++) {

		if (Routing_table_entry* entry = this->get_my_vector()->get_entry(i)) {
			logger->router_log("id: %d | cost: %d | next_hop: %d\n",
					entry->id, entry->cost, entry->next_hop);
		}

	}

}


void Routing_table::print_shortest_paths() {

	logger->router_log("SHORTEST PATHS\n");

	for (std::vector<Routing_table_entry>::iterator iter = shortest_paths.vector_entries->begin();
			iter != shortest_paths.vector_entries->end(); iter++) {

		logger->router_log("id: %d | cost: %d | next_hop: %d\n",
				(*iter).id, (*iter).cost, (*iter).next_hop);

	}

}









