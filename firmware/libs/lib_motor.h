/*
 * lib_motor.h
 *
 * Created by Michael Sokolsky
 * Last modified: 30 April 09
 */

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
// Number of bytes in a power packet (including header)
#define MOTOR_PWR_BYTES 3

#define MOTOR_PACKET_HEADER 0x7F
#define MOTOR_PWM_HEADER 0x7E
#define MOTOR_SPI_PADDING 0x80

// Rate of slew to new speed (ratio based on one step per cycle)
#define MOTOR_SLEW_RATE 1.0

// Time (in cycles) to timeout if no commands received
#define MOTOR_TIMEOUT 50

#define XSC100 (-1008) // cos(90+100) * 1024
#define YSC100 (-178) // sin(90+100) * 1024
#define TSC100 (1096) // 1.07 * 1024
#define XSC220 (658) // cos(90+220) * 1024
#define YSC220 (-784) // sin(90+220) * 1024
#define TSC220 (1096) // 1.07 * 1024
#define XSC340 (350) // cos(90+340) * 1024
#define YSC340 (962) // sin(90+340) * 1024
#define TSC340 (1096) // 1.07 * 1024

#define ABS(a) (((int)(a)) >= 0 ? (int)(a) : -(int)(a))

extern unsigned int power_rx_data[MOTOR_PWR_BYTES];

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
 * Sets the speed of the motors with slew-rate limit in
 * x-y-theta space;
 */
void motor_set_speed_xytheta(signed char xvel, signed char yvel, 
    signed char tvel);

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
 * Initialize power packet
 */
void power_init_packet();

/* 
 * Main motor event loop.  Sends speeds to motors and gets their status.
 * Also collects system voltage (for now, this should move to lib_power)
 */
int motor_event( void );

/*
 * Returns number of clicks (speed) for the last cycle of the motor
 */
signed char motor_clicks(int motor);

/*
 * Returns raw current sense value for the last cycle of the motor
 */
unsigned char motor_current(int motor);

/*
 * Returns raw temperature sense value for the last cycle of the motor
 */
unsigned char motor_temp(int motor);

/*
 * Returns the user commanded velocity of the motor
 */
signed char motor_command(int motor);
