
#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include <vector>

#include "network_structures.h"

class Router;

class Routing_table {

	typedef struct Entry {
		int cost;
		int next_hop;
	} Entry;

	std::vector<Routing_table_vector>* routing_table;

	Router* router;

public:
	Routing_table(Router* router,
			Control_message_init_payload* init_payload);
	~Routing_table();

	void update_cost(int router_id, int cost);
	void update_routing(Router_update_message* message);

	void recalculate_vector();
	int distance(int router_id_src, int router_id_dest);

	const std::vector<
		Routing_table_vector>* get_routing_table();

	Routing_table_vector* get_vector(int router_id);
	Routing_table_vector* get_my_vector();
	Routing_table_entry* get_my_vector_entry(int router_id);

	void print_routing_table();

	int routing_table_length;
	int my_router_id;
	char my_router_port [6];
	char my_data_port [6];

};


#endif
