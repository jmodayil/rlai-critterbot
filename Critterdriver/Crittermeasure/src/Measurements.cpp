#include "Measurements.h"

#include <iostream>
#include <fstream>

Chrono Measurements::chrono;
std::ofstream Measurements::logFile;
pthread_mutex_t Measurements::mutex = PTHREAD_MUTEX_INITIALIZER;

Measurements::Measurements() {
	logFile.open("/var/log/measures.txt", std::fstream::trunc);
	logFile << "observationSend actionReceived actionStartWriting actionWritten commandBackFromARM";
	for (int i = 0; i < PERIOD; i++)
		logFile << " startWaiting" << i  << " startReading" << i << " doneReading" << i;
	logFile << std::endl;
	time = -1;
	chrono.start();
}

Measurements::~Measurements() {
}

void Measurements::start() {
	if (time > 0)
		logLastData();
	lock();
	actionReceived = -1;
	actionWriting = -1;
	actionWritten = -1;
	commandBackFromARM = -1;
	observationSend = -1;
	chrono.start();
	for (int i = 0; i < PERIOD; i++) {
		startWaiting[i] = -1;
		startReading[i] = -1;
		doneReading[i] = -1;
	}
	time = 0;
	unlock();
}

void Measurements::logLastData() {
	logFile << observationSend << " "
			<< actionReceived << " "
			<< actionWriting << " "
			<< actionWritten << " "
			<< commandBackFromARM;
	for (int i = 0; i < PERIOD; i++)
		logFile << " " << startWaiting[i] << " " << startReading[i] << " " << doneReading[i];
	logFile << std::endl;
	logFile.flush();
}

void Measurements::setCommandBackFromARM() {
	commandBackFromARM = timeElapsed();
}

bool Measurements::isDone() {
	lock();
	bool result = time < 0 || time >= PERIOD;
	unlock();
	return result;
}

void Measurements::setStartWaiting() {
	if (isDone())
		return;
	startWaiting[time] = timeElapsed();
}

void Measurements::setStartReading() {
	if (isDone())
		return;
	startReading[time] = timeElapsed();
}

void Measurements::setDoneReading() {
	if (isDone())
		return;
	doneReading[time] = timeElapsed();
}

void Measurements::nextTimeStep() {
	time++;
}

void Measurements::setObservationSend() {
	observationSend = timeElapsed();
}

void Measurements::setActionReceived() {
	actionReceived = timeElapsed();
}

void Measurements::setActionStartWriting() {
	actionWriting = timeElapsed();
}

void Measurements::setActionWritten() {
	actionWritten = timeElapsed();
}

void Measurements::lock() {
	pthread_mutex_lock(&mutex);
}

void Measurements::unlock() {
	pthread_mutex_unlock(&mutex);
}

double Measurements::timeElapsed() {
	return chrono.timeElapsedNano() / 1e6;
}

int Measurements::getTime() {
	return time;
}
