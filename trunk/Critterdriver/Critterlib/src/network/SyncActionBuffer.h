#ifndef ACTIONPROVIDER_H_
#define ACTIONPROVIDER_H_

#include <pthread.h>
#include "../drops/CritterControlDrop.h"
#include "../utils/Chrono.h"
class SyncActionBuffer {
public:
	SyncActionBuffer();
	virtual const CritterControlDrop* getLastAction();
	virtual void pushAction(const CritterControlDrop& action);
	void setEnable(bool enabled);
	virtual ~SyncActionBuffer();
protected:
	static bool forceSendAction;
	static pthread_mutex_t controlDropMutex;
	CritterControlDrop lastControlDrop;
	int nbPendingActions;
private:
	CritterControlDrop bufferedControlDrop;
	Chrono chrono;
	unsigned long nbActionNotSend;
	unsigned long totalActionPushed;
	unsigned long totalActionDropped;
	bool disabled;
	void logActionsDropped(int wasPending, bool everythingIsFine);
};

#endif /* ACTIONPROVIDER_H_ */
