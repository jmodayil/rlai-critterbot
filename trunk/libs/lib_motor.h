
#include "armio.h"
#include "lib_error.h"
#include "lib_events.h"
#include "lib_spi.h"

#define MOTOR_NUM_MOTORS 3

#define MOTOR_MAX_SPEED 127

int motor_init( void );

void motor_set_speed(int motor, int speed); 

int motor_event( void );
