#ifndef MEASUREMENTS_H_
#define MEASUREMENTS_H_

#include <utils/Chrono.h>
#include <fstream>
#include <pthread.h>

#define PERIOD 5

class Measurements {
private:
	Measurements();
	~Measurements();
public:
	static Measurements& m() {
		static Measurements singleton;
		return singleton;
	}
public:
	void start();
	void setStartWaiting();
	void setStartReading();
	void setDoneReading();
	void setActionReceived();
	void setActionStartWriting();
	void setActionWritten();
	void setObservationSend();
	void setCommandBackFromARM();
	void nextTimeStep();
	bool isDone();
	int getTime();
private:
	void lock();
	void unlock();
	double timeElapsed();
	void logLastData();
	static pthread_mutex_t mutex;
	static Chrono chrono;
	static std::ofstream logFile;
private:
	int time;
	double actionReceived;
	double observationSend;
	double actionWriting;
	double actionWritten;
	double commandBackFromARM;
	double startWaiting[PERIOD];
	double startReading[PERIOD];
	double doneReading[PERIOD];
};

#endif /* MEASUREMENTS_H_ */
