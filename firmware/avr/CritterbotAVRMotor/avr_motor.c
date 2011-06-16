/*
 * avr_motor.c
 *
 *  Created on: Jun 5, 2009
 *      Author: sokolsky
 *
 * This code is most definietly NOT OO.  There is significant internal state
 * shared between functions.
 */

#include <avr/io.h>
#include "include/avr_motor.h"

// Maximum speed to allow the wheels to spin
#define SPEED_LIMIT 25
// KP and KD for the standard PID controller
#define KP 3
#define KD 4
// KP, KI, and KD for the soft PID controller
#define KPS 8
#define KIS 8
#define KDS 16
// Current limit
#define I_LIMIT 30
// Current limit for stalled motor condition
// MGB: changed to 2 to prevent robot paralysis
#define I_LIMIT_MIN 2
// Ratio for scaling down current in a stalled condition.  A rate of 1 will
// scale down to I_LIMIT_MIN in 255 cycles, higher rates multiply this.
#define I_LIMIT_RATE 6
// Bound the integral error term in
#define I_ERROR_MAX 1000
// Bound the maximum output of the PID controllers.  This is defined as
// 64 * 127, the maximum control value shifted by 6.
#define MAX_FIXED_POINT_OUTPUT 8128
// Maximum system voltage we allow the robot to move with.  In 1/10th of a volt
#define MAX_MOVE_VOLTAGE 185
// Maximum voltage for speed throttling
#define MAX_VOLTAGE_SETPOINT 100

// Absolute value
#define ABS(x) ((x) >= 0 ? (x) : -(x))

//static int8_t speed_limit_now;
static uint8_t i_limit_now;
static int32_t i_error;
static int16_t error;
extern volatile uint8_t current;
extern volatile uint8_t v_now;

// Maximum voltage to send to the motors.  This floats with the system bus
// voltage but may be decreased due to a extended current limit condition
int8_t voltage_limit;

volatile uint16_t count, old_count;
uint8_t rstate, event, event_count;
uint8_t adc_mux;
volatile uint8_t dat, dummy, current, temperature;
volatile uint8_t v_now;

void motor_init(void) {

  v_now = 255;
  error = 0;

  voltage_limit = 0;
  i_limit_now = I_LIMIT;
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

  // Leave one dead cycle if the velocity sign changes
  if((voltage >= 0 ? 1 : -1) != (last_voltage >= 0 ? 1 : -1)) {
    OCR0A = 0;
    OCR0B = 0;
    PORTD &= ~(MTR_LOW_A_PIN | MTR_LOW_B_PIN);
  }
  else {
    OCR0B = (voltage > 0) ?
      ((voltage == 127) ? 255 : 2 * (uint8_t)voltage) : 0;
    OCR0A = (voltage < 0) ?
      ((voltage <= -127) ? 255 : 2 * (256-(uint8_t)voltage)) : 0;
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

  static uint8_t in_limit;
  // Maximum voltage we can reach due to v_now bus voltage
  uint8_t max_volt;
  static int8_t correction;
  static uint8_t count;
  int8_t setpoint_sign = setpoint > 0 ? 1 : -1;
  // make setpoint a magnitude
  setpoint = ABS(setpoint);

  // If we are over the current limit
  if(current > I_LIMIT) {
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
  }

  // Decrease current to very small value if the motor is stalled.
  // This does not protect against a condition where the motor is spinning
  // but still using a large amount of current.
  if(in_limit < 255 && clicks == 0 && motor_setpoint != 0)
    in_limit++;
  if(in_limit > 0 && clicks != 0 && motor_setpoint != 0)
      in_limit--;
  if(in_limit > 0 && current > I_LIMIT_MIN) {
    count++;
    if(count >= I_LIMIT_RATE) {
      count = 0;
      if(voltage_limit > 1)
        voltage_limit--;
    }
  }
  else {
    if(voltage_limit < 127)
        voltage_limit++;
  }


  // Find the maximum safe voltage we can send to the motors given the bus
  // voltage.  This should cap the voltage to the motors at 12V, their rated
  // voltage, and disable motors if voltage is > 17.5V
  if(v_now <= 120)
    max_volt = 127;
  else if(v_now > MAX_MOVE_VOLTAGE)
    max_volt = 0;
  else
    max_volt = 127 - ((uint8_t)(v_now - 120)* 3) / 4;

  if(voltage_limit > max_volt)
    voltage_limit = max_volt;


  return (setpoint - correction) * setpoint_sign;
}

/**
 * Hard speed limit for voltage control mode.  Prevents large commands from
 * accelerating the motors past a certain speed.
 *
 * Relatively unstable when there is zero load on the motors.  With a little
 * load (i.e. weight of the robot) output is much smoother, however there is
 * plenty of room to improve this as it is quite rough.
 */
int8_t speed_limit(int8_t setpoint) {
  return (setpoint > MAX_VOLTAGE_SETPOINT) ? MAX_VOLTAGE_SETPOINT :
    (setpoint < -MAX_VOLTAGE_SETPOINT? -MAX_VOLTAGE_SETPOINT : setpoint);
  /*
  static uint8_t correction;

  int16_t error = ABS(clicks) - (SPEED_LIMIT << 2);

  if(error > 0) {
    correction += (error >> 4) + 1;
    if(correction > ABS(setpoint))
      correction = ABS(setpoint);
  }
  else if(correction > 0) {
    correction--;
  }

  return (setpoint >= 0) ? (setpoint - correction) : (setpoint + correction);
  */
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
  // Find the error, clicks is 4x as large as setpoint in scale
  error = (((int16_t)setpoint << 2) - clicks);
  // Find the error derivative
  d_error = (error - last_error);

  // Calculate the output by incrementing it by the P and D errors
  hires_speed += error * KP + d_error * KD;
  // Bound output
  if(hires_speed > MAX_FIXED_POINT_OUTPUT)
    hires_speed = MAX_FIXED_POINT_OUTPUT;
  if(hires_speed < -MAX_FIXED_POINT_OUTPUT)
    hires_speed = -MAX_FIXED_POINT_OUTPUT;

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

  // Use a decaying sum to approximate the integral error.  Decrease previous
  // sum by about 2% each cycle.  This brings it down to about 1/8th the
  // original value after 100 cycles.
  i_error = (i_error * 63) / 64 + error;
  if(i_error > I_ERROR_MAX)
    i_error = I_ERROR_MAX;
  if(i_error < -I_ERROR_MAX)
    i_error = -I_ERROR_MAX;

  // PID calculation and bounding
  hires_speed = error * KPS + i_error * KIS + d_error * KDS;
  if(hires_speed > MAX_FIXED_POINT_OUTPUT)
    hires_speed = MAX_FIXED_POINT_OUTPUT;
  if(hires_speed < -MAX_FIXED_POINT_OUTPUT)
    hires_speed = -MAX_FIXED_POINT_OUTPUT;

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
