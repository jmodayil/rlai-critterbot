#ifndef MEASUREDSERIALLINK_H_
#define MEASUREDSERIALLINK_H_

#include <seriallink/CritterSerialLink.h>

class MeasuredSerialLink : public CritterSerialLink {
public:
	MeasuredSerialLink(ActionWriter& actionWriter);
	virtual ~MeasuredSerialLink();
public:
	virtual CritterStateDrop* readStateDrop();
protected:
	virtual bool parseByte(unsigned char b);
private:
	bool nbParsedByte;
};

#endif /* MEASUREDSERIALLINK_H_ */
