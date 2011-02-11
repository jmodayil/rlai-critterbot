#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "LatencyLogger.h"

LatencyLogger::LatencyLogger() {
}

void LatencyLogger::processStateDrop(const CritterStateDrop & stateDrop) {
  clock_gettime(CLOCK_REALTIME, current);
  long diff;
  if (current->tv_nsec > last->tv_nsec)
  	  diff = current->tv_nsec - last->tv_nsec;
  else
	  diff = 1000000000 + current->tv_nsec - last->tv_nsec;
  periodLogFile << diff << std::endl;
  periodLogFile.flush();
  timespec* buffer = last;
  last = current;
  current = buffer;
}

void LatencyLogger::init() {
	periodLogFile.open("/var/log/period.txt");
	current = &t0;
	last = &t1;
	clock_gettime(CLOCK_REALTIME, last);
}

LatencyLogger::~LatencyLogger() {
	periodLogFile.flush();
	periodLogFile.close();
}
