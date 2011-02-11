#include "SyncActionBuffer.h"
#include <stdio.h>
#include <iostream>
#include <algorithm>

static bool const debugLog = false;
bool SyncActionBuffer::forceSendAction = false;

pthread_mutex_t SyncActionBuffer::controlDropMutex = PTHREAD_MUTEX_INITIALIZER;

SyncActionBuffer::SyncActionBuffer() {
	nbActionNotSend = 0;
	nbPendingActions = 0;
	totalActionPushed = 0;
	totalActionDropped = 0;
	disabled = true;
}

void SyncActionBuffer::setEnable(bool enabled) {
	pthread_mutex_lock(&controlDropMutex);
	disabled = !enabled;
	pthread_mutex_unlock(&controlDropMutex);
}

void SyncActionBuffer::logActionsDropped(int wasPending, bool everythingIsFine) {
	if (!debugLog || totalActionPushed == 0)
		return;
	static bool everythingWasFine = false;
	if (everythingWasFine && !everythingIsFine)
		std::cout << std::endl;
	double actionDroppedRatio = (double) totalActionDropped / totalActionPushed;
	if (!everythingIsFine || !everythingWasFine)
		std::cout << "Missing: " << nbActionNotSend << " Pending: "
				<< wasPending << " Dropped: " << totalActionDropped << "/"
				<< totalActionPushed << " (" << actionDroppedRatio * 100
				<< "%)" << std::endl;
	everythingWasFine = everythingIsFine;
}

const CritterControlDrop* SyncActionBuffer::getLastAction() {
	CritterControlDrop* result = NULL;
	bool everythingIsFine = true;
	pthread_mutex_lock(&controlDropMutex);
	int wasPending = nbPendingActions;
	if (forceSendAction || nbPendingActions > 0) {
		if (nbPendingActions > 0)
			bufferedControlDrop = lastControlDrop;
		else
			bufferedControlDrop.reset();
		result = &bufferedControlDrop;
	}
	if (!forceSendAction && !disabled) {
		totalActionPushed += nbPendingActions;
		if (nbPendingActions > 0)
			totalActionDropped += nbPendingActions - 1;
		else
			nbActionNotSend++;
		everythingIsFine = nbPendingActions == 1;
	}
	nbPendingActions = 0;
	pthread_mutex_unlock(&controlDropMutex);
	logActionsDropped(wasPending, everythingIsFine);
	return result;
}

void SyncActionBuffer::pushAction(const CritterControlDrop& action) {
	pthread_mutex_lock(&controlDropMutex);
	lastControlDrop = action;
	nbPendingActions++;
	pthread_mutex_unlock(&controlDropMutex);
}

SyncActionBuffer::~SyncActionBuffer() {
}
