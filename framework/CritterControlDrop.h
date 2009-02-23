#ifndef CritterControlDrop_H
#define CritterControlDrop_H

#include "Drop.h"
#include "CritterDrop.h"

#define NUM_LEDS 16

using namespace std;

/**
 * @todo comment me
 */
class CritterControlDrop : public CritterDrop {
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
  enum { NONE, CLEAR, BATTERY, BALL, ERROR, EMERGENCY, BUSY, CUSTOM } led_mode;
 
  struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
  } led_val[NUM_LEDS];

	CritterControlDrop();
	virtual ~CritterControlDrop();

	virtual int getSize();
	virtual void writeArray(void *d);
	virtual void readArray(void *d);
	virtual string toString(string offset = "");
	
  virtual string getName();

};

#endif
