/*
 * MeasuredActionWriter.cpp
 *
 *  Created on: 2010-12-10
 *      Author: degris
 */

#include "MeasuredActionWriter.h"
#include "Measurements.h"

MeasuredActionWriter::MeasuredActionWriter() {
}

MeasuredActionWriter::~MeasuredActionWriter() {
}

void MeasuredActionWriter::writeActionToSerialLink(){
	ActionWriter::writeActionToSerialLink();
	if (markedAction)
		Measurements::m().setActionWritten();
}

void MeasuredActionWriter::prepareActionData(const CritterControlDrop& controlDrop) {
	ActionWriter::prepareActionData(controlDrop);
	markedAction = controlDrop.m100 != 0;
}
