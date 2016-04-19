
#ifndef DAVIDTOW_INCLUDE_ROUTING_TABLE_H_
#define DAVIDTOW_INCLUDE_ROUTING_TABLE_H_

#include <vector>

class Routing_table {

	typedef struct Entry {
		int cost;
		int next_hop;
	} Entry;

	std::vector< std::vector<Entry> > table;

public:
	Routing_table(int num_of_routers);
	~Routing_table();

	void initialize_table();

};


#endif
