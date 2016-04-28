
#ifndef TIMER_H
#define TIMER_H

#include <list>

//#include "router.h"

struct Event {
	long seconds;
	long microseconds;
	int router_id;
};


class Router;
class Timer {

	std::list<Event> timed_events;
	struct timeval* tv;

	void set_timer_for_next_event();

	Router* router;
	int interval;

public:

	Timer(Router* router);
	void start(int interval);

	void schedule_alarm(float time, struct pkt packet);
	void handle_next_event_alarm();
	void remove_alarm(int seq_num);

};


#endif
