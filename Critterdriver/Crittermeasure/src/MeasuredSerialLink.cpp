#include "MeasuredSerialLink.h"

#include "Measurements.h"

MeasuredSerialLink::MeasuredSerialLink(ActionWriter& actionWriter) : CritterSerialLink(actionWriter) {

}

MeasuredSerialLink::~MeasuredSerialLink() {
}


CritterStateDrop* MeasuredSerialLink::readStateDrop() {
	nbParsedByte = 0;
	return CritterSerialLink::readStateDrop();
}

bool MeasuredSerialLink::parseByte(unsigned char b) {
	if (nbParsedByte == 0)
		Measurements::m().setStartReading();
	nbParsedByte++;
	return CritterSerialLink::parseByte(b);
}
