/*
 * Chrono.h
 *
 *  Created on: 2010-11-26
 *      Author: degris
 */

#ifndef CHRONO_H_
#define CHRONO_H_

#include <unistd.h>
#include <stdlib.h>
#include <time.h>

class Chrono {
public:
	Chrono();
	virtual ~Chrono();
	void start();
	double timeElapsed();
	long timeElapsedNano();
private:
	timespec startedTime;
};

#endif /* CHRONO_H_ */
