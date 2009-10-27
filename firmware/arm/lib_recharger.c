/*
 * lib_recharger.c
 *
 * Created by Marc G. Bellemare 
 * Last modified: 27 August 2009
 *
 * This file defines a simple finite state automaton designed to find the
 *  the battery charger and dock to it.
 */

#include "lib_recharger.h"
#include "lib_random.h"
#include "lib_motor.h"
#include "lib_adcspi.h"
#include "lib_mi.h"
#include "lib_leddrive.h"

event_s recharger_event_s = {
  recharger_init,
  recharger_event,
  0
};

// control variables
// 0 is the most recent bit pf history
unsigned char irHistory[IR_LIGHT_SIZE][IR_HISTORY_SIZE];
// Between -255 and 255
int beaconSensor[IR_LIGHT_SIZE];
// Could be removed as it is a fn of beaconSensor and irHistory[0]
int lastBeaconSensor[IR_LIGHT_SIZE];

// State information
int rotationComponent;
unsigned char fsaState;
unsigned char lastFSAState;
char convolutionFactor;

unsigned char counter;
unsigned char wanderCounter;
unsigned char wanderDirection;
unsigned char immobileCounter;

char recharger_enabled;

int recharger_init() {
  recharger_enabled = 0;
  // Reset the system to the initial state
  return recharger_reset();
}

int recharger_reset() {
  int i, j;

  convolutionFactor = 1;
  wanderCounter = 0;

  for (i = 0; i < IR_LIGHT_SIZE; i++) {
    for (j = 0; j < IR_HISTORY_SIZE; j++) {
      irHistory[i][j] = 0;
    }

    beaconSensor[i] = 0;
    lastBeaconSensor[i] = 0;
  }

  // We first set the fsaState so that the last state points to FSA_WANDER and
  //  not some random junk
  fsaState = FSA_WANDER;
  recharger_enter_state(FSA_WANDER);
  rotationComponent = 1;

  return 0;
}

void recharger_enable(void) {
  recharger_enabled = 1;
  // Prevent interference from the MI
  mi_disable_commands();
  // Make sure we can actually go and recharge
  motor_enable_charging();
}

void recharger_disable(void) { 
  recharger_enabled = 0;
  mi_enable_commands();
}

int recharger_event() {
  #if 0
  // Commented out until the charger function works; may be moved to
  //  lib_monitor.c
  if (!recharger_enabled && 
      motor_get_voltage() < RECHARGER_LOW_VOLTAGE_TRIGGER) {
    recharger_reset();
    recharger_enable();
  }
  // Disable the recharger routine once the voltage goes up again 
  else if (recharger_enabled && motor_get_voltage() > 
    RECHARGER_LOW_VOLTAGE_TRIGGER + RECHARGER_VOLTAGE_HYSTERESIS) {
    recharger_disable();
  }
  #endif

  if (recharger_enabled) {
    // Run the FSA every so often
    if (++counter >= CF_EVENT_INTERVAL) {
      recharger_step();
      counter = 0;
    }

    // Set the LEDs to 'busy' ... this may not be the best place for this
    leddrive_busy();
  }
  return 0;
}

void recharger_enter_state(int newState) { 
  lastFSAState = fsaState;
  fsaState = newState;
}

void recharger_step() {
  // First update the IR and convolution data
  recharger_update_history();

  // Take an action based on the current state
  // @todo This could be an array!
  switch (fsaState) {
    case FSA_WANDER:
      recharger_fsa_wander();
      break;
    case FSA_ALIGN:
      recharger_fsa_align();
      break;
    case FSA_FORWARD:
      recharger_fsa_forward();
      break;
    case FSA_CLOSE:
      recharger_fsa_close();
      break;
    case FSA_ROTATE_DOCK:
      recharger_fsa_rotate_dock();
      break;
    case FSA_DONE:
      recharger_fsa_done();
      break;
  }
}

void recharger_fsa_wander() {
  // If we can see the beacon, start moving towards it
  if (recharger_sees_beacon())
    // Note: we may not be aligned with the becaon, in which case we will
    //  first align ourselves with it (but this gets called from recharger_fsa_forward)
    // The reason is to set up lastFSAState properly
    recharger_enter_state(FSA_FORWARD);
  else {
    if (wanderCounter <= 0) {
        // Pick a new, random direction
        wanderDirection = armrandom() & 3;
        wanderCounter = CF_WANDER_LENGTH;
    }

    int x, y;
    switch (wanderDirection) {
      case 0: // Forward
        x = FSA_FORWARD_VELOCITY; y = 0;
        break;
      case 1: // Backward
        x = -FSA_FORWARD_VELOCITY; y = 0;
        break;
      case 2: // Left (I might be getting these reversed)
        x = 0; y = FSA_FORWARD_VELOCITY;
        break;
      case 3: // Right
        x = 0; y = -FSA_FORWARD_VELOCITY;
        break;
      default:
        wanderDirection = 0;
        x = y = 0;
        break;
    }

    // Move in the given direction, but avoid walls
    recharger_act_avoid(x, y, 0); 
  }
}

int recharger_sees_beacon() {
  int beaconDirection = recharger_beacon_dir();

  return (beaconSensor[beaconDirection] >= IR_MINIMUM_THRESHOLD);
}

int recharger_beacon_dir() {
  int maxBeacon = 0;
  int maxBeaconValue = beaconSensor[0];
  int i, s;

  for (i = 1; i < IR_LIGHT_SIZE; i++) {
    s = beaconSensor[i];
    if (s > maxBeaconValue) {
      maxBeaconValue = s;
      maxBeacon = i;
    }
  }

  return maxBeacon;
}

void recharger_fsa_align() {
  // Find the direction of the beacon (max. beacon value)
  int beaconDirection = recharger_beacon_dir();

  if (beaconDirection == FSA_TARGET_SENSOR) { // If aligned, drive forward
    recharger_enter_state(lastFSAState); // No action
  }
  else if (beaconDirection <= 4) // Rotate right
    recharger_act_avoid(0, 0, FSA_ROTATE_VELOCITY);
  else // Rotate left
    recharger_act_avoid(0, 0, -FSA_ROTATE_VELOCITY);
}

void recharger_fsa_forward() {
  // If the beacon disappears, wander around
  if (!recharger_sees_beacon())
    recharger_enter_state(FSA_WANDER);
  if (recharger_beacon_dir() != FSA_TARGET_SENSOR)
    recharger_enter_state(FSA_ALIGN);
  if (beaconSensor[FSA_TARGET_SENSOR] >= IR_SATURATION_THRESHOLD) {
    immobileCounter = 5;
    recharger_enter_state(FSA_CLOSE);
  }
  else {
    recharger_act_forward(FSA_FORWARD_VELOCITY, FSA_MAX_ROTATION_COMPONENT, 1);
  }
}

void recharger_act_forward(int forwardVel, int rotationVel, 
  int avoidObjects) {
  // Determine the rotational component to add to the forward motion
  int beaconGradient = 
    beaconSensor[FSA_TARGET_SENSOR] - lastBeaconSensor[FSA_TARGET_SENSOR];

  int rotationSign = (rotationComponent >= 0? 1: -1);

  // Rotate towards a positive value of the beacon sensor; if the gradient
  //  is negative, this means reversing direction
  rotationComponent = rotationSign * beaconGradient;

  // @todo use constants
  if (beaconGradient < FSA_GRADIENT_THRESHOLD && 
      beaconGradient >= -FSA_GRADIENT_THRESHOLD)
      rotationComponent = 0;
  else if (rotationComponent > rotationVel)
    rotationComponent = rotationVel; 
  else if (rotationComponent < -rotationVel)
    rotationComponent = -rotationVel;

  if (avoidObjects)
    recharger_act_avoid(forwardVel, 0, -rotationComponent);
  else
    recharger_act(forwardVel, 0, -rotationComponent);
}

/** Returns true if any of the motors has non-zero velocity. */
int recharger_is_moving() {
  int i;

  for (i = 0; i < MOTOR_NUM_MOTORS; i++)
    if (motor_clicks(i) != 0) return 1;

  return 0;
}

/** Similar to recharger_fsa_forward, but we move slower and enter the rotate-dock state
  *  once we stop moving. */ 
void recharger_fsa_close() {
  if (!recharger_sees_beacon())
    recharger_enter_state(FSA_WANDER);
  if (recharger_beacon_dir() != FSA_TARGET_SENSOR) {
    fsaState = FSA_ALIGN;
  }
  else if (!recharger_is_moving()) {
    if (--immobileCounter <= 0) {
      recharger_enter_state(FSA_ROTATE_DOCK);
      immobileCounter = 5;
    }
  }
  else
    // Forward, but don't try to avoid obstacles
    recharger_act_forward(FSA_SLOW_FORWARD_VELOCITY, 
      FSA_MAX_ROTATION_COMPONENT, 0);
}

void recharger_fsa_rotate_dock() {
  int isDocked = (motor_get_voltage() > SHORE_POWER_MIN_VOLTAGE);
  if (isDocked) {
    recharger_enter_state(FSA_DONE);
    immobileCounter = 0;
  }
  else if (!recharger_is_moving()) {
    if (--immobileCounter <= 0) {
      recharger_enter_state(FSA_DONE);
      immobileCounter = 0;
    }
  }
  else // Rotate left
    recharger_act(0, 0, -FSA_SLOW_ROTATE_VELOCITY);
}

void recharger_fsa_done() {
  // Disable routine after a little while
  if (++immobileCounter >= 20) {
    recharger_disable();
  }
}

void recharger_update_history() {
  int beaconValue;
  int i, j, beaconSum;
  int halfHistorySize = IR_HISTORY_SIZE / 2;

  // Update the IR history and frequency data
  for (i = 0; i < IR_LIGHT_SIZE; i++) {
    // Shift all the values (lazy)
    for (j = IR_HISTORY_SIZE-1; j > 0; j--) {
      irHistory[i][j] = irHistory[i][j-1];
    }

    irHistory[i][0] = adcspi_get_output(IR_LIGHT_ADC_DEVICE, i);
    
    lastBeaconSensor[i] = beaconSensor[i];

    // Compute the new convolution values
    beaconSum = 0;
    for (j = 0; j < halfHistorySize * 2; j += 2) {
      beaconSum += irHistory[i][j];
      beaconSum -= irHistory[i+1][j];
    }

    // Drop the last history bit if we have an odd number of steps

    // Compute the value as the absolute average signal 
    beaconValue = beaconSum / halfHistorySize;
    beaconSensor[i] = ABS(beaconValue);
  }

  convolutionFactor *= -1;
}

/** A map of IR sensor to x, y, theta offsets. Each of these is added if the
  *  corresponding IR distance sensor is above a threshold.
  */
char avoid_offsets[IR_DISTANCE_SIZE][3] = {
  { -FSA_AVOID_VELOCITY, 0, 0 },
  { -FSA_AVOID_VELOCITY, - FSA_AVOID_VELOCITY/2, 0 },
  { 0, -FSA_AVOID_VELOCITY/2, 0 },
  { 0, -FSA_AVOID_VELOCITY/2, 0 },
  { 0, FSA_AVOID_VELOCITY/2, 0 },
  { 0, FSA_AVOID_VELOCITY/2, 0 },
  { -FSA_AVOID_VELOCITY, FSA_AVOID_VELOCITY/2, 0 },
  { 0, 0, FSA_AVOID_ROTATE_VELOCITY },
  { 0, 0, FSA_AVOID_ROTATE_VELOCITY },
  { 0, 0, -FSA_AVOID_ROTATE_VELOCITY } };

void recharger_act_avoid(int x, int y, int theta) {
    int i;
  
    // Modify the requested x, y, theta velocities based on the ir distance
    //  sensor readings
    for (i = 0; i < IR_DISTANCE_SIZE; i++) {
      unsigned char value = adcspi_get_output(IR_DISTANCE_ADC_DEVICE, i);
      if (value > IR_DISTANCE_AVOID_THRESHOLD) {
        x += avoid_offsets[i][0];
        y += avoid_offsets[i][1];
        theta += avoid_offsets[i][2];
      }
    }

    recharger_act(x, y, theta);
}

void recharger_act(int x, int y, int theta) {
    motor_set_speed_xytheta(x, y, theta);
}


