
#ifndef ROUTER_SOCKET_DEFINITIONS_H
#define ROUTER_SOCKET_DEFINITIONS_H

#include <vector>


typedef struct Routing_table_entry {
	unsigned short id;
	unsigned short next_hop;
	unsigned short cost;
} Routing_table_entry;


typedef struct Routing_table_vector {
	unsigned int router_ip;
	unsigned short router_port;
	unsigned short data_port;
	unsigned short id;
	Routing_table_entry* get_entry(int router_id) {
		for (std::vector<Routing_table_entry>::iterator iter = this->vector_entries->begin();
				iter != this->vector_entries->end(); iter++) {
			if ((*iter).id == router_id) {
				return (Routing_table_entry*)&(*iter);
			}
		}

		return 0;
	}
	std::vector<Routing_table_entry>* vector_entries;
} Routing_table_vector;


typedef struct Router_update_entry {
	unsigned int router_ip;
	unsigned short router_port;
	unsigned short id;
	unsigned short cost;
} Router_update_entry;


typedef struct Router_update_message {
	unsigned short num_of_update_fields;
	unsigned int router_ip;
	unsigned short router_port;
	Router_update_entry* update_entries;
} Router_update_message;


#endif
