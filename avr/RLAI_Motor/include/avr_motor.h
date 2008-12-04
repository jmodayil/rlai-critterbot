#ifndef AVR_MOTOR_H
#define AVR_MOTOR_H

#define MTR_LOW_A_PIN 	_BV(1)
#define MTR_LOW_B_PIN	_BV(0)
#define MTR_EN_PIN 	_BV(4)
#define MTR_A_PIN 	_BV(5)
#define MTR_B_PIN 	_BV(6)
#define MOTOR_OFF	127


int8_t motor_setpoint;
int16_t limited_drive_speed;
int16_t clicks;

void motor_init(void);
void set_speed(int8_t);
int8_t power_limit(int8_t);
int8_t current_limit(int8_t);
int8_t pid_control(int8_t);

#endif
