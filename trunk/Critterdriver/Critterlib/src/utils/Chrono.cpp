#include "Chrono.h"

Chrono::Chrono() {
	start();
}

Chrono::~Chrono() {
}

void Chrono::start() {
	clock_gettime(CLOCK_REALTIME, &startedTime);
}

double Chrono::timeElapsed() {
	timespec current;
	clock_gettime(CLOCK_REALTIME, &current);
	long diff = current.tv_nsec;
	diff = diff - startedTime.tv_nsec; //ensure signed subtraction
	return current.tv_sec - startedTime.tv_sec + (.000000001 * diff);
}

long Chrono::timeElapsedNano() {
	timespec current;
	clock_gettime(CLOCK_REALTIME, &current);
	long diff;
	if (current.tv_nsec > startedTime.tv_nsec)
		diff = current.tv_nsec - startedTime.tv_nsec;
	else
		diff = 1000000000 + current.tv_nsec - startedTime.tv_nsec;
	return diff;
}
