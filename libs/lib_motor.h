
#include "armio.h"
#include "lib_error.h"
#include "lib_events.h"
#include "lib_spi.h"

#define MOTOR_NUM_MOTORS 3

// This limited the value you can set the motors to, and also
// provides scaling for incoming commands
#define MOTOR_MAX_SPEED 100

#define MOTOR_MAX_PWM 127
// Number of bytes in a motor packet (including header)
#define MOTOR_NUM_BYTES 5

#define MOTOR_PACKET_HEADER 0x7F
#define MOTOR_PWM_HEADER 0x7E

// Time (in cycle) to slew to new speed
#define MOTOR_SLEW_TIME 100

/*
 * Initialize motor driver
 */
int motor_init( void );

/*
 * Sets the speed of motor, within +/-MOTOR_MAX_SPEED
 */
void motor_set_speed(int motor, signed char speed);

/*
 * Sets the speed of motors, within +/-MOTOR_MAX_SPEED
 * with a slew-rate limit on the change
 */
void motor_set_speed_slew(signed char speed100,
    signed char speed220, signed char speed340);

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

/*
 * Returns number of clicks (speed) for the last cycle of the motor
 */
char motor_clicks(int motor);

/*
 * Returns raw current sense value for the last cycle of the motor
 */
unsigned char motor_current(int motor);

/*
 * Returns raw temperature sense value for the last cycle of the motor
 */
unsigned char motor_temp(int motor);
