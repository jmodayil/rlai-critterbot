#ifndef CritterControlDrop_H
#define CritterControlDrop_H

#include "Drop.h"

using namespace std;

/**
 * @todo comment me
 */
class CritterControlDrop : public DataDrop {
 public:
	static string name;
  enum Motor_Mode { WHEEL_SPACE, XYTHETA_SPACE };
	// whatever the data structure is going to be
  Motor_Mode motor_mode;
  union {
    int m100_vel;
    int x_vel;
  };
  union {
    int m220_vel;
    int y_vel;
  };
  union {
    int m340_vel;
    int theta_vel;
  };
  // leaving this aside for now
  //enum { THING1, THING2, THING3} led_mode;
  
	CritterControlDrop();
	virtual ~CritterControlDrop();

	virtual int getSize();
	virtual void writeArray(void *data);
	virtual void readArray(void *data);
	virtual string toString(string offset = "");
	virtual string getName();

};

#endif
