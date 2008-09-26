
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

int motor_init( void );

void motor_set_speed(int motor, int speed); 
void motor_set_pwm(int motor, int pwm);

unsigned char motor_get_voltage();
void motor_init_packet(int motor);
int motor_event( void );
