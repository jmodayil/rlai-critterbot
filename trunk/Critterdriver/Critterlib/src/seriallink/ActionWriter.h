#ifndef ACTIONWRITER_H_
#define ACTIONWRITER_H_

#include "../drops/CritterControlDrop.h"

#define SER_HEADER1 0xDE
#define SER_HEADER2 0xAD
#define SER_HEADER3 0xBE
#define SER_HEADER4 0xEF

class ActionWriter {
public:
	ActionWriter();
	virtual ~ActionWriter();
	bool start(int fid);
	void writeActionsMainLoop();
	bool sendActionIfPossible(const CritterControlDrop& controlDrop);
protected:
	virtual void writeActionToSerialLink();
	virtual void prepareActionData(const CritterControlDrop& controlDrop);
private:
	bool isActionWritten();
	void setDataWritten(bool value);
private:
	pthread_t actionWriterThread;
	int fid;
	static unsigned char motorData[10];
	static unsigned char ledData[NUM_LEDS * 3];
	static CritterControlDrop::LedMode ledMode;
	static bool dataWritten;
	static pthread_mutex_t flag_mutex;
	static pthread_mutex_t condition_mutex;
	static pthread_cond_t  condition_cond;
};

#endif /* ACTIONWRITER_H_ */
