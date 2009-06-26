/*
 * avr_motor.c
 *
 *  Created on: Jun 5, 2009
 *      Author: sokolsky
 */

#include <avr/io.h>
#include "include/avr_motor.h"

#define SPEED_LIMIT 25
#define KP 3 //8
#define KD 4 //50
#define KPS 8
#define KIS 8
#define KDS 16
#define P_LIMIT 500
#define I_LIMIT 25
#define I_LIMIT_MIN 1
#define I_LIMIT_SCALE 32
#define I_LIMIT_RATE 3
// What is this?
#define P_SCALE 3
#define E_SCALE 4
#define I_HIST_SIZE 100
#define STALL_THRESH 20

//static int8_t speed_limit_now;
static uint8_t i_limit_now;
static int32_t i_error;
static uint8_t i_hist[I_HIST_SIZE];
static uint8_t *i_hist_loc;
static uint16_t p_avg;
static int16_t error;
extern volatile uint8_t current;
extern volatile uint8_t v_now;

volatile uint16_t count, old_count;
uint8_t rstate, event, event_count;
uint8_t adc_mux;
volatile uint8_t dat, dummy, current, temperature;
volatile uint8_t v_now;


void motor_init(void) {

  v_now = 255;
  error = 0;

  i_limit_now = I_LIMIT;
  //speed_limit_now = SPEED_LIMIT;
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

  static int8_t correction;
  static uint8_t count;
  int16_t error;
  int8_t setpoint_sign = setpoint > 0 ? 1 : -1;
  // make setpoint a magnitude
  setpoint = setpoint > 0 ? setpoint : -setpoint;

  error = i_limit_now - current;

  // If we are over the current limit
  if(error < 0) {
    correction++;
    if(correction >= setpoint)
      correction = setpoint - 1;
    if(correction < 0)
      correction = 0;
    count++;
    if(count >= I_LIMIT_RATE) {
      count = 0;
      //if(speed_limit_now > 1)
      //    speed_limit_now--;
    }
  }
  else {
    if(correction > 0)
      correction--;
    if(correction < 0)
      correction = 0;
    else {
      //if(speed_limit_now < SPEED_LIMIT)
      //  speed_limit_now++;
      count = 0;
    }
  }
  //if(speed_limit_now <= 0)
  //  return 0;
  return (setpoint - correction) * setpoint_sign;
}

/*
int8_t current_limit(int8_t setpoint) {

  static uint8_t cycle_count;
  static int16_t correction;
  static int8_t last_error;
  int8_t d_error, error;

  error = i_limit_now - current;
  d_error = error - last_error;
  last_error = error;

  // If we're going less than half as fast as we are supposed to be...
  // Decrease the current limit slowly until min
  if(((clicks >> 1) * (clicks > 0 ? 1 : -1)) <
      setpoint > 0 ? setpoint : -setpoint) {
    if(cycle_count++ > 10) {
      cycle_count = 0;
      i_limit_now--;
      if(i_limit_now < I_LIMIT_MIN)
        i_limit_now = I_LIMIT_MIN;
    }
  }
  // Otherwise increase it back to the normal limit
  else {
    if(cycle_count > 0)
      cycle_count--;
    else {
      if(i_limit_now < I_LIMIT)
        i_limit_now++;
    }
  }
  if(error < 0) {
    //d_error = error - last_error;
    correction += error;
    if(correction < ((int16_t)(setpoint > 0 ? -setpoint : setpoint)) * I_LIMIT_SCALE )
      correction = ((int16_t)(setpoint > 0 ? -setpoint : setpoint)) * I_LIMIT_SCALE ;
  }
  else{
    correction += error;
    if(correction > 0)
      correction = 0;
  }

  setpoint += (correction / I_LIMIT_SCALE) * (setpoint >= 0 ? 1 : -1);
  i_error += (correction / I_LIMIT_SCALE) * (setpoint >= 0 ? 1 : -1);

  return setpoint;

}*/

int8_t power_limit(int8_t setpoint) {

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
  else {
    return setpoint;
  }

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

int8_t soft_pid_control(int8_t setpoint) {

  int8_t final_speed;
  int16_t d_error;
  static int16_t last_error;
  static int16_t hires_speed;


  if(setpoint > 100 || setpoint < -100 || setpoint == 0)
    return 0;
  if(setpoint > SPEED_LIMIT)
    setpoint = SPEED_LIMIT;
  if(setpoint < -SPEED_LIMIT)
    setpoint = -SPEED_LIMIT;

  last_error = error;
  error = (((int16_t)setpoint << 2) - clicks);
  d_error = (error - last_error);
  //i_error += (error - i_error) / 8;
  i_error = (i_error * 63) / 64 + error;

  hires_speed = error * KPS + i_error * KIS + d_error * KDS;
  if(hires_speed > 8128)
    hires_speed = 8128;
  if(hires_speed < -8128)
    hires_speed = -8128;

  return hires_speed >> 6;
  //final_speed = hires_speed >> 6;
  // Bound output speed.
  /*if(final_speed > 0) {
    if(final_speed > speed_limit_now)
      final_speed = speed_limit_now;
  } else {
    if(final_speed < -speed_limit_now)
      final_speed = -speed_limit_now;
  }*/
  //return final_speed;
}
