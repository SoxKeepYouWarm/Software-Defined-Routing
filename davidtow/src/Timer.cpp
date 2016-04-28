
#include "Timer.h"

struct Event {
	float alarm_time;
	struct pkt packet; // CHANGE THIS LATER
};

class Timer {

	std::list<Event> timed_events;
	float time_buffer;

	void set_timer_for_next_event();

public:

	Timer();

	void schedule_alarm(float time, struct pkt packet);
	void handle_next_event_alarm();
	void remove_alarm(int seq_num);

};


Timer::Timer() {
	time_buffer = 0;
}


void Timer::schedule_alarm(float time, struct pkt sent_packet) {
	// TODO switch to using packets
	float alarm_time = get_sim_time() + time;

	struct Event new_event;
	new_event.alarm_time = alarm_time;


	new_event.packet = sent_packet;

	// check if this is the only event
	if (timed_events.empty()) {

		// if no events scheduled already, push and start timer immediately
		timed_events.push_back(new_event);
		set_timer_for_next_event();

	} else {

		// push event to the back of the queue
		timed_events.push_back(new_event);

	}

}


void Timer::set_timer_for_next_event() {

	Event next_event = timed_events.front();
	float time_to_next_alarm = next_event.alarm_time - get_sim_time();
	starttimer(FROM_A, time_to_next_alarm);
	printf("SET_TIMER_fOR_NEXT_EVENT: seqnum: %d alarm_time: %f\n", next_event.packet.seqnum, time_to_next_alarm);

}


void Timer::handle_next_event_alarm() {

	// get event, and pop from queue
	if ( ! timed_events.empty() ) {
		Event triggered_event = timed_events.front();
		timed_events.pop_front();

		std::cout << "HANDLE_EVENT: seqnum: " << triggered_event.packet.seqnum << std::endl;

		// if another event is ahead of the timeout, start timer.
		if ( ! timed_events.empty() ) {
			set_timer_for_next_event();
		}

		// otherwise schedule alarm will trigger this event
		// upon noticing that the queue is empty

		// set alarm for packet
		schedule_alarm(TIME_A, triggered_event.packet);

		// send packet out
		printf("A_OUTPUT: resending packet seqnum: %d\n", triggered_event.packet.seqnum);
		tolayer3(FROM_A, triggered_event.packet);
		printf("DEBUG: CALLED AFTER TOLAYER3\n");

	} else {
		printf("event queue empty, no event to be handled :(\n");
	}

}


void Timer::remove_alarm(int seq_num) {

	printf("REMOVE_ALARM: seqnum: %d\n", seq_num);

	int i = 0;
	for (std::list<struct Event>::iterator iter = timed_events.begin(); iter != timed_events.end(); std::advance(iter, 1)) {

		if (iter->packet.seqnum == seq_num) {
			printf("REMOVE_ALARM: alarm found\n");

			// if the timer is running this alarm, need to set timer to next event
			int reset_timer = (i == 0);

			iter = timed_events.erase(iter);
			iter --;

			if (reset_timer) {
				printf("REMOVE_ALARM: timer needs to be stopped for removed alarm\n");
				stoptimer(FROM_A);

				if ( ! timed_events.empty() ) {
					printf("REMOVE_ALARM: timer needs to be set for next event\n");
					set_timer_for_next_event();
				} else {
					printf("REMOVE_ALARM: event queue is empty, timer will not be restarted\n");
				}
			} else {
				printf("REMOVE_ALARM: timer does not need to be reset\n");
			}

			return;
		}

		i++;

	}

	printf("REMOVE_ALARM: FAILED\n");

}

