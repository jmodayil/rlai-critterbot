#ifndef CritterControlDrop_H
#define CritterControlDrop_H

#define NUM_LEDS 16

#define AVR_ENABLE_CHARGING 0x01
#define AVR_DISABLE_CHARGING 0x02

#include <string>

class CritterControlDrop {
public:
	CritterControlDrop();
	virtual ~CritterControlDrop();
	int getDropSize();
	void readArray(void *d);
	void reset();
public:
	static std::string name;

	enum Motor_Mode {
		WHEEL_SPACE, XYTHETA_SPACE, WHEEL_VOLTAGE
	};
	// whatever the data structure is going to be
	Motor_Mode motor_mode;
	union {
		int m100;
		int x_vel;
	};
	union {
		int m220;
		int y_vel;
	};
	union {
		int m340;
		int theta_vel;
	};
	enum LedMode {
		NONE, CLEAR, BATTERY, BALL, ERROR, EMERGENCY, BUSY, CUSTOM
	} led_mode;

	// NOT currently passed around (temporary!) 28/10/09 MGB
	int avr_commands;

	struct {
		unsigned char r;
		unsigned char g;
		unsigned char b;
	} led_val[NUM_LEDS];
private:
	int getDataSize();
};

#endif
