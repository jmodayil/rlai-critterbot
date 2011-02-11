#ifndef CRITTERSTATEDROP_H_
#define CRITTERSTATEDROP_H_

#define IR_DISTANCE_SIZE 10
#define IR_LIGHT_SIZE    8
#define LIGHT_SIZE       4
#define THERMAL_SIZE     8
#define BUMP_SIZE        32

#include <string>
#include "USeconds.h"

class CritterStateDrop {
public:
	CritterStateDrop();
	virtual ~CritterStateDrop();
	void writeArray(void *d) const;
	int getDropSize() const;
public:
	static std::string name;

	USeconds time;

	enum Data_Source {
		ROBOT, SIMULATOR, LOGFILE
	} data_source;
	enum Power_Source {
		SHORE, BATTERY
	} power_source;

	unsigned int bus_voltage;
	enum Charge_State {
		UNPLUGGED,
		CHARGE_40 = 2,
		CHARGE_40_160 = 4,
		CHARGE_160 = 5,
		CHARGE_160_280 = 7,
		CHARGE_280 = 8,
		CHARGE_TOPOFF = 9,
		CHARGE_COMPLETE = 10,
		CHARGE_ERROR = 200
	} charge_state;
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
	unsigned int light[LIGHT_SIZE]; // 30
	unsigned int thermal[THERMAL_SIZE];
	unsigned int bump[BUMP_SIZE];
	unsigned int error_flags; // (4) 34
	unsigned int cycle_time;
	unsigned int monitor_state;
private:
	int getDataSize() const;
};

#endif /* CRITTERSTATEDROP_H_ */
