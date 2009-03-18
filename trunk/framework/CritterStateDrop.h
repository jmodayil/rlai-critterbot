#ifndef CritterStateDrop_H
#define CritterStateDrop_H

#include "Drop.h"
#include "CritterDrop.h"

#define IR_DISTANCE_SIZE 10
#define IR_LIGHT_SIZE    8
#define LIGHT_SIZE       4
#define THERMAL_SIZE     8
#define BUMP_SIZE        32

using namespace std;

class CritterStateDrop : public CritterDrop {
 public:
  static string name;
  
  enum Power_Source { SHORE, BAT40, BAT160, BAT280 } power_source;

  unsigned int bus_voltage;
  unsigned int batv40;
  unsigned int batv160;
  unsigned int batv280;
  struct {
    int command;
    int velocity;
    int current;
    int temp;
  } motor100, motor220, motor340; // 9

  struct {
    int x;
    int y;
    int z;
  } accel, mag; // 15

  int rotation; // 16
  unsigned int ir_distance[IR_DISTANCE_SIZE]; // 26
  unsigned int ir_light[IR_LIGHT_SIZE]; 
  unsigned int light[LIGHT_SIZE];  // 30
  unsigned int thermal[THERMAL_SIZE];
  unsigned int bump[BUMP_SIZE];
  unsigned int error_flags; // (4) 34
  unsigned int cycle_time;

  CritterStateDrop();
  virtual ~CritterStateDrop();

  virtual int getSize();
  virtual void writeArray(void *d);
  virtual void readArray(void *d);
  virtual string toString(string offset = "");

  virtual string getName() { return name; }
};

#endif

