#include "Routing_table.h"


Routing_table::Routing_table(int num_of_routers) {

	table = std::vector< std::vector<Entry> >(num_of_routers,
			std::vector<Entry>(num_of_routers));

}


Routing_table::~Routing_table() {

}


void Routing_table::initialize_table() {

}
