
#include "armio.h"
#include "lib_error.h"
#include "lib_events.h"
#include "lib_spi.h"

#define MOTOR_NUM_MOTORS 3

#define MOTOR_MAX_SPEED 100
#define MOTOR_MAX_PWM 127
#define MOTOR_NUM_BYTES 5

#define MOTOR_PACKET_HEADER 0x7F
#define MOTOR_PWM_HEADER 0x7E

/*
 * Initialize motor driver
 */
int motor_init( void );

/*
 * Sets the speed of motor, within +/-MOTOR_MAX_SPEED
 */
void motor_set_speed(int motor, int speed);

/*
 * Sets a raw PWM motor value in range +/-127
 */
void motor_set_pwm(int motor, int pwm);

/*
 * Returns the system voltage in 1/4 Volt increments. (12V = 48)
 * Returns 255 if voltage readings are not operating correctly.
 */
unsigned char motor_get_voltage();

/*
 * Initialize a motor packet
 */
void motor_init_packet(int motor);

/* 
 * Main motor event loop.  Sends speeds to motors and gets their status.
 * Also collects system voltage (for now, this should move to lib_power)
 */
int motor_event( void );
