/*
 * MeasuredActionWriter.h
 *
 *  Created on: 2010-12-10
 *      Author: degris
 */

#ifndef MEASUREDACTIONWRITER_H_
#define MEASUREDACTIONWRITER_H_

#include <seriallink/ActionWriter.h>

class MeasuredActionWriter : public ActionWriter {
public:
	MeasuredActionWriter();
	virtual ~MeasuredActionWriter();
protected:
	virtual void writeActionToSerialLink();
	virtual void prepareActionData(const CritterControlDrop& controlDrop);
private:
	bool markedAction;
};

#endif /* MEASUREDACTIONWRITER_H_ */
