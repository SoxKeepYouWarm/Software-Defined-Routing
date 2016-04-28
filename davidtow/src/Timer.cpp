
#include "router.h"
#include "Timer.h"


Timer::Timer(Router* router): router(router), interval(0) {
	this->tv = &router->tv;
}


void Timer::start(int interval) {

	this->interval = interval;
	// broadcast my vector table to all neighbors

}
