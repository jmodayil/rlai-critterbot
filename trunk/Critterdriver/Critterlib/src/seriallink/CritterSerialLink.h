/*
 * CritterSerialLink.h
 *
 *  Created on: Nov 24, 2010
 *      Author: thomas
 */

#ifndef CRITTERSERIALLINK_H_
#define CRITTERSERIALLINK_H_

#define STATE_LENGTH 79

#define BAUDRATE B115200
#define DEVICE "/dev/ttyS0"

#include <list>
#include <termios.h>
#include "ActionWriter.h"
#include "../drops/CritterStateDrop.h"

class CritterSerialLink {
public:
	CritterSerialLink(ActionWriter& actionWriter);
	virtual ~CritterSerialLink();
	bool init();
	virtual CritterStateDrop* readStateDrop();
	bool sendActionIfPossible(const CritterControlDrop& controlDrop);
	void waitForData();
	void close();
protected:
	virtual bool parseByte(unsigned char b);
private:
	CritterStateDrop stateDrop;
	ActionWriter& actionWriter;
	void initport();
	CritterStateDrop* parseDataToDrop(unsigned char buf[]);
	void synchronizeData();
private:
	int fid;
	struct termios oldterm;
	unsigned char data[STATE_LENGTH];
};

#endif /* CRITTERSERIALLINK_H_ */
