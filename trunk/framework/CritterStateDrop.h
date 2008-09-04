#ifndef CritterStateDrop_H
#define CritterStateDrop_H

#include "Drop.h"

using namespace std;

class CritterStateDrop : public DataDrop {
 public:
	static string name;

  enum Power_Source { SHORE, BAT40, BAT160, BAT280 } power_source;

  unsigned char bus_voltage;
  unsigned char batv40;
  unsigned char batv160;
  unsigned char batv280;
  struct {
    int velocity;
    int current;
    int temp;
  } motor100, motor220, motor340;

  struct {
    int x;
    int y;
    int z;
  } accel, mag;

  int rotation;
  unsigned int ir_distance[10];
  unsigned int ir_light[8];
  unsigned int light[4];
  unsigned int thermal[8];
  unsigned int bump[32];
  unsigned int error_flags;

	CritterStateDrop();
	virtual ~CritterStateDrop();

	virtual int getSize();
	virtual void writeArray(void *data);
	virtual void readArray(void *data);
	virtual string toString(string offset = "");
	virtual string getName();

};

#endif

