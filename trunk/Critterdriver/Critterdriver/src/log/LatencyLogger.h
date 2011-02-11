/*
 * LatencyLogger.h
 *
 *  Created on: 2010-11-24
 *      Author: degris
 */

#ifndef LATENCYLOGGER_H_
#define LATENCYLOGGER_H_

#include <fstream>
#include <drops/CritterStateDrop.h>

class LatencyLogger {
public:
	LatencyLogger();
	void init();
	void processStateDrop(const CritterStateDrop& stateDrop);
	virtual ~LatencyLogger();
private:
	std::ofstream periodLogFile;
	timespec t0, t1;
	timespec* current;
	timespec* last;
};

#endif /* LATENCYLOGGER_H_ */
