/*
 * avr_motor.c
 *
 *  Created on: Jun 5, 2009
 *      Author: sokolsky
 */

#include <avr/io.h>
#include "include/avr_motor.h"

// Maximum speed to allow the wheels to spin
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
static int16_t error;
extern volatile uint8_t current;
extern volatile uint8_t v_now;

// Maximum voltage to send to the motors.  This floats with the system bus
// voltage but may be decreased due to a extended current limit condition
volatile int8_t voltage_limit;

volatile uint16_t count, old_count;
uint8_t rstate, event, event_count;
uint8_t adc_mux;
volatile uint8_t dat, dummy, current, temperature;
volatile uint8_t v_now;


void motor_init(void) {

  v_now = 255;
  error = 0;

  voltage_limit = 127;
  i_limit_now = I_LIMIT;
  i_limit_now = SPEED_LIMIT;
  motor_setpoint = 101;
  PORTD &= ~(MTR_EN_PIN|MTR_A_PIN|MTR_B_PIN|MTR_LOW_A_PIN|MTR_LOW_B_PIN);

  DDRD |= (MTR_EN_PIN|MTR_A_PIN|MTR_B_PIN|MTR_LOW_A_PIN|MTR_LOW_B_PIN);

  TCCR0A = 0xA1;
  TCCR0B = 0x01;

  OCR0A = 0;
  OCR0B = 0;

  PORTD |= MTR_EN_PIN;
}

/**
 * DO NOT MESS WITH ANYTHING IN THIS FUNCTION UNLESS YOU KNOW WHAT YOU ARE DOING
 * Unwise changes to the code in set_voltage can cause permanent and probably
 * smelly damage to the robot.
 *
 * This sets the output to the motor control H-Bridge.  Turning on both the
 * high and low side drivers on the same leg at the same time will cause a
 * nasty short.  There is no hardware protection against doing this.
 */
void set_voltage(int8_t voltage) {

  static int8_t last_voltage;

  // Bound voltage by the voltage limit
  if(voltage > voltage_limit)
    voltage = voltage_limit;
  if(voltage < -voltage_limit)
    voltage = -voltage_limit;

  // Leave one dead cycle if velocity sign changes
  if((voltage >= 0 ? 1 : -1) != (last_voltage >= 0 ? 1 : -1)) {
    OCR0A = 0;
    OCR0B = 0;
    PORTD &= ~(MTR_LOW_A_PIN | MTR_LOW_B_PIN);
  }
  else {
    OCR0B = (voltage > 0) ?
      ((voltage == 127) ? 255 : 2 * (uint8_t)voltage) : 0;
    OCR0A = (voltage < 0) ?
      ((voltage == -127) ? 255 : 2 * (256-(uint8_t)voltage)) : 0;
    if( voltage < 0 ) {
      PORTD &= ~MTR_LOW_A_PIN;
      PORTD |= MTR_LOW_B_PIN;
    }
    else if(voltage > 0 ){
      PORTD &= ~MTR_LOW_B_PIN;
      PORTD |= MTR_LOW_A_PIN;
    }
    else
      PORTD &= ~(MTR_LOW_A_PIN | MTR_LOW_B_PIN);
  }
  last_voltage = voltage;
}

int8_t current_limit(int8_t setpoint) {

  static int8_t correction;
  static uint8_t count;
  int16_t error;
  int8_t setpoint_sign = setpoint > 0 ? 1 : -1;
  // make setpoint a magnitude
  setpoint = setpoint > 0 ? setpoint : -setpoint;

  // calculate difference between limit and actual current
  error = i_limit_now - current;

  // If we are over the current limit
  if(error < 0) {
    // Increase the correction factor by one
    correction++;
    // Bound the correction
    if(correction >= setpoint)
      correction = setpoint - 1;
    if(correction < 0)
      correction = 0;
  }
  // If we are not over the current limit
  else {
    // If there is still correction going on slowly ease off
    if(correction > 0)
      correction--;
    // Bound correction for safety in case other code changes that might cause
    // a negative correction, which makes thing very very bad
    if(correction < 0)
      correction = 0;
    else {
      if(voltage_limit < 127)
        voltage_limit++;
      if(voltage_limit > 127)
        voltage_limit = 127;
      count = 0;
    }
  }

  if(correction > 0) {
    //count++;
    //if(count >= I_LIMIT_RATE) {
      //count = 0;
      if(voltage_limit > 1)
        voltage_limit--;
    //}
  }
  else {
    if(voltage_limit < 127)
      voltage_limit++;
  }

  //if(speed_limit_now <= 0)
  //  return 0;
  return (setpoint - correction) * setpoint_sign;
}

/**
 * Traditional velocity PID control, implements the integral component by
 * increasing or decreasing the output by the result of KP*Perror + KD*Derror.
 * This provides very fast and accurate response, but the integral component
 * is not particularly adjustable.
 */
int8_t pid_control(int8_t setpoint) {

  int16_t d_error;
  static int16_t last_error;
  static int16_t hires_speed;

  if(setpoint > 100 || setpoint < -100 || setpoint == 0)
    return 0;

  last_error = error;
  // Find the error and shift left for fixed point math.
  error = (((int16_t)setpoint << 2) - clicks);
  // Find the error derivative
  d_error = (error - last_error);

  // Calculate the output by incrementing it by the P and D errors
  hires_speed += error * KP + d_error * KD;
  // Bound output
  if(hires_speed > 8128)
    hires_speed = 8128;
  if(hires_speed < -8128)
    hires_speed = -8128;

  // Scale back down from fixed point to integer for output.
  return hires_speed >> 6;

}

/**
 * A velocity PID controller that actually implements the integral component
 * as a normal PID controller would.  This means that the output will never
 * actually reach the setpoint but will get arbitrarily close depending on the
 * value of KI.
 */
int8_t soft_pid_control(int8_t setpoint) {

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

  // Use a decaying sum to approximate the integral error.
  // @TODO calculate if this needs to be bounded.
  i_error = (i_error * 63) / 64 + error;

  // PID calculation and bounding
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
