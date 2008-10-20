#include <avr/io.h>
#include "include/avr_motor.h"

#define KP 4 //8
#define KD 25 //50
#define SLEW_STEPS 10
#define P_LIMIT 800
#define P_SCALE 3
#define E_SCALE 4
#define I_HIST_SIZE 100
#define STALL_THRESH 20

static uint8_t i_hist[I_HIST_SIZE];
static uint8_t *i_hist_loc;
static uint16_t p_avg;
static int16_t error;
extern volatile uint8_t current;
extern volatile uint8_t v_now;

void motor_init(void) {

	v_now = 48;
	i_hist_loc = i_hist;

	motor_setpoint = 101;
	PORTD &= ~(MTR_EN_PIN|MTR_A_PIN|MTR_B_PIN|MTR_LOW_A_PIN|MTR_LOW_B_PIN);

	DDRD |= (MTR_EN_PIN|MTR_A_PIN|MTR_B_PIN|MTR_LOW_A_PIN|MTR_LOW_B_PIN);

	TCCR0A = 0xA1;
	TCCR0B = 0x01;

	OCR0A = 0;
	OCR0B = 0;

	PORTD |= MTR_EN_PIN;
}

void set_speed(int8_t speed) {

	static int8_t last_speed;

	if((speed >= 0 ? 1 : -1) != (last_speed >= 0 ? 1 : -1)) {
		OCR0A = 0;
		OCR0B = 0;
		PORTD &= ~(MTR_LOW_A_PIN | MTR_LOW_B_PIN);
	}
	else {
		OCR0A = (speed > 0) ?
			((speed == 127) ? 255 : 2 * (uint8_t)speed) : 0;
		OCR0B = (speed < 0) ?
			((speed == -127) ? 255 : 2 * (256-(uint8_t)speed)) : 0;
		if( speed > 0 ) {
			PORTD &= ~MTR_LOW_A_PIN;
			PORTD |= MTR_LOW_B_PIN;
		}
		else if(speed < 0 ){
			PORTD &= ~MTR_LOW_B_PIN;
			PORTD |= MTR_LOW_A_PIN;
		}
		else
			PORTD &= ~(MTR_LOW_A_PIN | MTR_LOW_B_PIN);
	}
	last_speed = speed;
}

int8_t current_limit(int8_t setpoint) {
	
	uint8_t i, avg;
	uint16_t sum;
	int16_t p_error, correction;
	int8_t limit_setpoint;
	static int8_t decay;

	*i_hist_loc++ = current;
	if(i_hist_loc >= i_hist + I_HIST_SIZE)
		i_hist_loc = i_hist;

	if(setpoint > 100 || setpoint < -100)
		return setpoint;

	sum = 0;
	for(i = 0; i < I_HIST_SIZE; i++)
		sum += i_hist[i];
	avg = sum / I_HIST_SIZE;
	p_avg = avg * v_now;

	if(p_avg > (4 * P_LIMIT)) {
		decay = 255;
		return 0;
	}
	else if(decay) {
		decay--;
		return 0;
	}
	else
		return setpoint;

	p_error = p_avg - P_LIMIT;
	
	if(p_error > 0) {	
		
		if(p_error > 0)
			correction = (p_error >> P_SCALE);// + ((error > 0 ? error : -error) >> E_SCALE);

		if(motor_setpoint > 0) {
			if(correction > setpoint)
				correction = setpoint;
			limit_setpoint = setpoint - correction;
		}
		else if(motor_setpoint < 0) {
			if(correction > -setpoint)
				correction = -setpoint;
			limit_setpoint = setpoint + correction;
		}
		else
			limit_setpoint = setpoint;	
		
	}
	else {
		limit_setpoint = setpoint;
	}
	return limit_setpoint;
	
}

int8_t pid_control(int8_t setpoint) {
	
	int16_t d_error;
	static int16_t last_error;
	static int16_t hires_speed;

	if(setpoint > 100 || setpoint < -100 || setpoint == 0)
		return 0;

	last_error = error;
	error = (((int16_t)setpoint << 2) - clicks);
	d_error = (error - last_error);

	hires_speed += error * KP + d_error * KD;
	if(hires_speed > 8128)
		hires_speed = 8128;
	if(hires_speed < -8128)
		hires_speed = -8128;

	return hires_speed >> 6;

}

