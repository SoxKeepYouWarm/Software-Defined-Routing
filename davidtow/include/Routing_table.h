
#ifndef DAVIDTOW_INCLUDE_ROUTING_TABLE_H_
#define DAVIDTOW_INCLUDE_ROUTING_TABLE_H_

#include <vector>

#include "network_structures.h"

class Routing_table {

	typedef struct Entry {
		int cost;
		int next_hop;
	} Entry;

	std::vector<std::vector
			<Routing_table_entry> >* routing_table;

public:
	Routing_table(Control_message_init_payload* init_payload);
	~Routing_table();

	void update(int router_id, int cost);

	const std::vector<
			std::vector<Routing_table_entry> >*
			get_routing_table();


	int routing_table_length;
	int my_router_id;
	char my_router_port [6];
	char my_data_port [6];

};


#endif
