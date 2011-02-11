#include "MocActionBuffer.h"

#include <iostream>
#include "Measurements.h"

MocActionBuffer::MocActionBuffer() {
	forceSendAction = true;
}

MocActionBuffer::~MocActionBuffer() {
}

void MocActionBuffer::pushAction(const CritterControlDrop& action) {
	if (action.m100 != 0)
		Measurements::m().setActionReceived();
	SyncActionBuffer::pushAction(action);
}
