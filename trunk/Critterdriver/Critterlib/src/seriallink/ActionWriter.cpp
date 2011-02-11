#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "ActionWriter.h"

CritterControlDrop::LedMode ActionWriter::ledMode;

void *writeAction(void *ptr) {
	((ActionWriter*) ptr)->writeActionsMainLoop();
	return NULL;
}

unsigned char ActionWriter::motorData[10] = { SER_HEADER1, SER_HEADER2,
		SER_HEADER3, SER_HEADER4, 0, 0, 0, 0, 0, 0 };
unsigned char ActionWriter::ledData[NUM_LEDS * 3];
bool ActionWriter::dataWritten = true;
pthread_mutex_t ActionWriter::flag_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ActionWriter::condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ActionWriter::condition_cond = PTHREAD_COND_INITIALIZER;

ActionWriter::ActionWriter() {
}

ActionWriter::~ActionWriter() {
}

bool ActionWriter::start(int fid) {
	this->fid = fid;
	int ret = pthread_create(&actionWriterThread, NULL, writeAction, this);
	if (ret != 0) {
		perror("error starting action thread");
		return false;
	}
	return true;
}

void ActionWriter::prepareActionData(const CritterControlDrop& controlDrop) {
	setDataWritten(false);
	motorData[4] = (char) controlDrop.motor_mode;
	motorData[5] = (char) controlDrop.x_vel;
	motorData[6] = (char) controlDrop.y_vel;
	motorData[7] = (char) controlDrop.theta_vel;
	ledMode = controlDrop.led_mode;
	motorData[8] = (char) controlDrop.led_mode;
	// AVR commands are disabled for now
	motorData[9] = 0;
	if (controlDrop.led_mode == CritterControlDrop::CUSTOM) {
		for (int i = 0; i < NUM_LEDS; i++) {
			ledData[3 * i] = controlDrop.led_val[i].r;
			ledData[3 * i + 1] = controlDrop.led_val[i].g;
			ledData[3 * i + 2] = controlDrop.led_val[i].b;
		}
	}
}

bool ActionWriter::isActionWritten() {
	bool result;
	pthread_mutex_lock(&flag_mutex);
	result = dataWritten;
	pthread_mutex_unlock(&flag_mutex);
	return result;
}

bool ActionWriter::sendActionIfPossible(const CritterControlDrop& controlDrop) {
	if (!isActionWritten())
		return false;
	prepareActionData(controlDrop);
	pthread_mutex_lock(&condition_mutex);
	pthread_cond_signal(&condition_cond);
	pthread_mutex_unlock(&condition_mutex);
	return true;
}

void ActionWriter::setDataWritten(bool value) {
	pthread_mutex_lock(&flag_mutex);
	dataWritten = value;
	pthread_mutex_unlock(&flag_mutex);
}

void ActionWriter::writeActionToSerialLink() {
	if (10 != write(fid, &motorData, 10))
		perror("Error writing motor data out to serial port!");
	if (ledMode == CritterControlDrop::CUSTOM) {
		if (NUM_LEDS * 3 != write(fid, &ledData, NUM_LEDS * 3))
			perror("Error writing led data out to serial port!");
	}
//	if (ioctl(fid, TCFLSH, TCOFLUSH) != 0)
//		perror("Error setting the serial for flushing write");
//	if (tcflush(fid, TCOFLUSH) != 0)
//		perror("Error flushing write");
	setDataWritten(true);
}

void ActionWriter::writeActionsMainLoop() {
	while (true) {
		writeActionToSerialLink();
		pthread_mutex_lock(&condition_mutex);
		pthread_cond_wait(&condition_cond, &condition_mutex);
		pthread_mutex_unlock(&condition_mutex);
	}
}

