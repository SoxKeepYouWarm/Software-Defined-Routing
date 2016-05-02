#include "Timer.h"

#include <iostream>

#include "router.h"
#include "Router_socket_manager.h"
#include "Routing_table.h"


Timer::Timer(Router* router): router(router), interval(0) {
	this->tv = &router->tv;
}


void Timer::start(long interval, int my_router_id) {

	this->interval = interval;

	// set initial timeval timeout
	tv->tv_sec = interval;
	tv->tv_usec = 0;

	// broadcast my vector table to all neighbors

	Timer_event my_update_event;

	// this is the first event being registered so use interval as timeout value
	calculate_remaining_time(&my_update_event);
	my_update_event.router_id = my_router_id;
	my_update_event.received_update = 0;
	my_update_event.missed_updates = 0;

	registered_events.push_back(my_update_event);

}


void Timer::register_event(int router_id) {

	Timer_event new_event;

	new_event.router_id = router_id;
	calculate_remaining_time(&new_event);
	new_event.received_update = 0;
	new_event.missed_updates = 0;

	registered_events.push_back(new_event);

}


void Timer::handle_timeout() {

	if (registered_events.empty()) {
		std::cout << "ERROR: HANDLE_TIMEOUT: "
				<< "called when registered_events is empty" << std::endl;
		return;
	}


	Timer_event popped_event = registered_events.front();
	registered_events.pop_front();

	// handle event
	if (popped_event.router_id == router->get_my_router_id()) {
		// my router needs to broadcast

		router->router_socket_manager->broadcast_vector_table();

		std::cout << "TIMER: broadcasting my vector table" << std::endl;

		// reset timer, add back to list
		calculate_remaining_time(&popped_event);
		registered_events.push_back(popped_event);

	} else {

		if (popped_event.received_update) {

			std::cout << "TIMER: router_id: " << popped_event.router_id
					<< " timed out after receiving update" << std::endl;

			// reset missed_update counter, unset received flag
			popped_event.missed_updates = 0;
			popped_event.received_update = 0;

			// reset timer, add back to list
			calculate_remaining_time(&popped_event);
			registered_events.push_back(popped_event);

		} else {

			std::cout << "TIMER: router_id: " << popped_event.router_id
					<< " timed out without receiving an update" << std::endl;

			popped_event.missed_updates ++;

			if (popped_event.missed_updates == 3) {
				// missed three consecutive updates from a router

				std::cout << "TIMER: router_id: " << popped_event.router_id
						<< " is inactive, setting cost to infinity"
						<< std::endl;

				// set router cost to infinity
				router->routing_table->get_my_vector_entry(popped_event.router_id)->cost = INFINITY;

				// event is not added back to the list

			} else {

				// reset timer, add back to list
				popped_event.seconds = interval;
				popped_event.microseconds = 0;
				calculate_remaining_time(&popped_event);
				registered_events.push_back(popped_event);

			}

		}

	}

	std::cout << "HANDLE_TIMEOUT: setting timer for next event" << std::endl;

	struct Timer_event next_event = registered_events.front();
	tv->tv_sec = next_event.seconds;
	tv->tv_usec = next_event.microseconds;

}


void Timer::calculate_remaining_time(Timer_event* event) {

	long clocked_seconds = tv->tv_sec;
	long clocked_micro_seconds = tv->tv_usec;

	if (clocked_seconds > 0 || clocked_micro_seconds > 0) {

		if (clocked_micro_seconds > 0) {
			event->seconds = interval - clocked_seconds - 1;
			event->microseconds = 1000000 - clocked_micro_seconds;
		} else {
			event->seconds = interval - clocked_seconds;
			event->microseconds = 0;
		}

	} else {
		event->seconds = interval;
		event->microseconds = 0;
	}

	std::cout << "CALCULATE_REMAINING_TIME: "
			<< "seconds: " << event->seconds
			<< " microseconds: " << event->microseconds << std::endl;

}



void Timer::notify_routing_update_received(int router_id) {

	for (std::list<Timer_event>::iterator iter = registered_events.begin();
			iter != registered_events.end(); iter++) {

		if ((*iter).router_id == router_id) {
			(*iter).received_update = 1;
		}

	}

}










