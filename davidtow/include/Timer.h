
#ifndef TIMER_H
#define TIMER_H

#include <list>

typedef struct Timer_event {
	long seconds;
	long microseconds;
	int router_id;
	int received_update;
	int missed_updates;
} Timer_event;


class Router;
class Timer {

	std::list<Timer_event> registered_events;
	struct timeval* tv;

	void calculate_remaining_time(Timer_event* event);

	Router* router;
	long interval;

public:

	Timer(Router* router);
	void start(long interval, int my_router_id);

	void register_event(int router_id);
	void handle_timeout();

};


#endif
