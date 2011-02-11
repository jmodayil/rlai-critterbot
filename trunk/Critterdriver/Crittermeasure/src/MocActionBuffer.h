/*
 * MocActionBuffer.h
 *
 *  Created on: 2010-12-07
 *      Author: degris
 */

#ifndef MOCACTIONBUFFER_H_
#define MOCACTIONBUFFER_H_

#include <network/SyncActionBuffer.h>

class MocActionBuffer : public SyncActionBuffer {
public:
	MocActionBuffer();
	virtual ~MocActionBuffer();
protected:
	virtual void pushAction(const CritterControlDrop& action);
};

#endif /* MOCACTIONBUFFER_H_ */
