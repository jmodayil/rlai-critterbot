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

int beaconFrequency[IR_LIGHT_SIZE];

// State information
int rotationComponent;
unsigned char fsaState;
unsigned char lastFSAState;
char convolutionFactor;
int time;
int chouillard=0;

unsigned char counter;
short wanderCounter;
unsigned char wanderDirection;
unsigned char immobileCounter;

char recharger_enabled;
char alignAvoid;

int recharger_init() {
  recharger_enabled = 0;
  // Reset the system to the initial state
  return recharger_reset();
}

int recharger_reset() {
  int i, j;

  convolutionFactor = 1;
  wanderCounter = CF_WANDER_LENGTH;

  for (i = 0; i < IR_LIGHT_SIZE; i++) {
    for (j = 0; j < IR_HISTORY_SIZE; j++) {
      irHistory[i][j] = 0;
    }

    beaconSensor[i] = 0;
    lastBeaconSensor[i] = 0;
    beaconFrequency[i] = 0;
  }
	
   counter = 0;
   time = 0;   
   alignAvoid = 1;
  
  // We first set the fsaState so that the last state points to FSA_WANDER and
  //  not some random junk
  fsaState = FSA_WANDER;
  recharger_enter_state(FSA_WANDER);
  rotationComponent = 1;

  return 0;
}

void recharger_enable(void) {
  recharger_enabled = 1;
  time = 0;
  // Prevent interference from the MI
  mi_disable_commands();
  // Make sure we can actually go and recharge
  // @todo re-enable
  //motor_enable_charging();
  
  // Set the LEDs to 'busy' ... this may not be the best place for this
  leddrive_busy();
}

void recharger_disable(void) { 
  recharger_enabled = 0;
  mi_enable_commands();
  leddrive_clear();
}

int recharger_event() {

  if (recharger_enabled) {
    // Run the FSA every so often
    if (++counter >= CF_EVENT_INTERVAL) {
      recharger_step();
      counter = 0;
    }

  }
  return 0;
}

void recharger_enter_state(int newState) { 
  lastFSAState = fsaState;
  fsaState = newState;

  switch (newState) {
    case FSA_WANDER:
      wanderCounter = CF_WANDER_LENGTH;
      // Fall through
    case FSA_ALIGN:
      alignAvoid = 1;
      break;
    case FSA_ROTATE:
      immobileCounter = CF_ROTATE_DOCK_IMMOBILE_TIMEOUT;
      wanderCounter = CF_ROTATE_DOCK_FAIL_TIMEOUT;
      break;
    case FSA_DOCK:
      immobileCounter = CF_ROTATE_DOCK_IMMOBILE_TIMEOUT;
      wanderCounter = CF_ROTATE_DOCK_FAIL_TIMEOUT;
      break;
  }

}


void recharger_step() {
  // First update the IR and convolution data
  recharger_update_history();

  // Do nothing until we have one full history of data
  if (++time < IR_HISTORY_SIZE) return;

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
    case FSA_ROTATE:
      recharger_fsa_rotate();
      break;
    case FSA_DOCK:
      recharger_fsa_dock();
      break;
  }
}

void recharger_fsa_wander() {
  // If we can see the beacon, start moving towards it
  if (recharger_sees_beacon() && wanderCounter == 0)
    // Note: we may not be aligned with the becaon, in which case we will
    //  first align ourselves with it (but this gets called from recharger_fsa_forward)
    // The reason is to set up lastFSAState properly
    recharger_enter_state(FSA_FORWARD);
  else {
    if (wanderCounter <= 0) {
        // Pick a new, random direction
        wanderDirection = armrandom() % 5;
        wanderCounter = CF_WANDER_LENGTH;
    }

    wanderCounter--;
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
      case 5:
        x = 0; y = 0;
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
  else {
    int rotVelocity;
    if (beaconDirection <= 4) // Rotate right
      rotVelocity = FSA_ROTATE_VELOCITY;
    else // Rotate left
      rotVelocity = -FSA_ROTATE_VELOCITY;
    
    if (alignAvoid)
      recharger_act_avoid(0, 0, rotVelocity);
    else
      recharger_act(0, 0, rotVelocity);
  }
}

void recharger_fsa_forward() {
  // If the beacon disappears, wander around
  if (!recharger_sees_beacon())
    recharger_enter_state(FSA_WANDER);
  else if (beaconSensor[FSA_TARGET_SENSOR] >= IR_SATURATION_THRESHOLD) {
    immobileCounter = 5;
    recharger_enter_state(FSA_CLOSE);
  }
  else if (recharger_beacon_dir() != FSA_TARGET_SENSOR)
    recharger_enter_state(FSA_ALIGN);
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
    alignAvoid = 0;
  }
  else if (!recharger_is_moving()) {
    if (--immobileCounter <= 0) {
      recharger_enter_state(FSA_ROTATE);
//      immobileCounter = 5;
    }
  }
  else
    // Forward, but don't try to avoid obstacles
    recharger_act_forward(FSA_SLOW_FORWARD_VELOCITY, 
      FSA_MAX_ROTATION_COMPONENT, 0);
}

void recharger_fsa_rotate() {
  if (1) {
      recharger_enter_state(FSA_DOCK);
	return;
    }

  if (!recharger_is_moving()) {
    if (--immobileCounter <= 0) {
      recharger_enter_state(FSA_DOCK);
    }
  }
  /* else if (getBeaconDirection() == 2)
    enterFSAState(FSA_DOCK); */
  // If we haven't stopped after a while, assume a fail
  else if (--wanderCounter <= 0) {
    recharger_enter_state(FSA_WANDER);
  }
  else // Rotate left 
    recharger_act(0, 0, -FSA_SLOW_ROTATE_VELOCITY);

}

void recharger_fsa_dock() {
 int isDocked = (motor_get_voltage() > SHORE_POWER_MIN_VOLTAGE);

  if (isDocked) {
    recharger_enter_state(FSA_DONE);

    return;
  }
  else if (!recharger_is_moving()) {
    if (--immobileCounter <= 0) {
      recharger_enter_state(FSA_WANDER);
    }
  }
  // Give up after a while if we couldn't dock 
  else if (--wanderCounter <= 0) {
    recharger_enter_state(FSA_WANDER);
  }
  else // If all is good, reset the immobile counter
    immobileCounter = CF_ROTATE_DOCK_IMMOBILE_TIMEOUT;

	chouillard = 0;
  // Lateral movement into the dock 
  if (chouillard <= 20)
    recharger_act(0, -FSA_SLOW_FORWARD_VELOCITY, 0);
  else if (chouillard <= 40)
    recharger_act(0, 0, -FSA_SLOW_ROTATE_VELOCITY);
  else // Temporary power-limiting workaround
    recharger_act(0, 0, 0);

  chouillard++;
  if (chouillard > 60) chouillard = 0;
}


void recharger_fsa_done() {
  // Disable routine after a little while
  if (++immobileCounter >= 20) {
    recharger_disable();
  }
}

void recharger_update_history() {
  int i, j;
  int halfHistorySize = IR_HISTORY_SIZE / 2;
  int c, oldConvolutionFactor;
  int v;

  for (i = 0; i < IR_LIGHT_SIZE; i++) {
    // Determine whether we should add or remove the last value
    oldConvolutionFactor = convolutionFactor * 
      ((IR_HISTORY_SIZE % 2)?-1:1);

    // Shift all the values (lazy)
    for (j = IR_HISTORY_SIZE-1; j > 0; j--) {
      irHistory[i][j] = irHistory[i][j-1];
      beaconFrequency[i] = 0;
    }

    irHistory[i][0] = adcspi_get_output(IR_LIGHT_ADC_DEVICE, i);
    
    c = 1;
    for (j = 0; j < IR_HISTORY_SIZE; j++) {
      beaconFrequency[i] += irHistory[i][j] * c;
      c = -c;
    }
    
    lastBeaconSensor[i] = beaconSensor[i];
    // For now, the convolution is not picking up the beacon signal, so we use
    //  raw intensities
    v = beaconFrequency[i] / halfHistorySize;
    beaconSensor[i] = ABS(v);
  }



  convolutionFactor *= -1;
}

/** A map of IR sensor to x, y, theta offsets. Each of these is added if the
  *  corresponding IR distance sensor is above a threshold.
  
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
*/

void recharger_act_avoid(int x, int y, int theta) {
  int i;

  int baseX, baseY, baseTheta;
  char hasAvoid = 0;

  baseX = x;
  baseY = y;
  baseTheta = theta;
  x = y = theta = 0;
  

 // Wall avoidance code, taken from Photovore by Mike Sokolsky
    if(adcspi_get_output(IR_DISTANCE_ADC_DEVICE, 0) > IR_DISTANCE_AVOID_THRESHOLD) {
      x += FSA_AVOID_VELOCITY;
      y += 0;
      hasAvoid = 1;
    }
    if(adcspi_get_output(IR_DISTANCE_ADC_DEVICE, 1) > IR_DISTANCE_AVOID_THRESHOLD) {
      x += FSA_AVOID_VELOCITY;
      y += FSA_AVOID_VELOCITY/2;
      hasAvoid = 1;
    }
    if(adcspi_get_output(IR_DISTANCE_ADC_DEVICE, 2) > IR_DISTANCE_AVOID_THRESHOLD) {

      x += 0;
      y += FSA_AVOID_VELOCITY/2;
      hasAvoid = 1;
    }
    if(adcspi_get_output(IR_DISTANCE_ADC_DEVICE, 3) > IR_DISTANCE_AVOID_THRESHOLD) {

      x += 0;
      y += FSA_AVOID_VELOCITY/2;
      hasAvoid = 1;
    }
    if(adcspi_get_output(IR_DISTANCE_ADC_DEVICE, 4) > IR_DISTANCE_AVOID_THRESHOLD) {

      x += 0;
      y -= FSA_AVOID_VELOCITY/2;
      hasAvoid = 1;
    }
    if(adcspi_get_output(IR_DISTANCE_ADC_DEVICE, 5) > IR_DISTANCE_AVOID_THRESHOLD) {
      x += 0;
      y -= FSA_AVOID_VELOCITY/2;
      hasAvoid = 1;
    }
    if(adcspi_get_output(IR_DISTANCE_ADC_DEVICE, 6) > IR_DISTANCE_AVOID_THRESHOLD) {
      x += FSA_AVOID_VELOCITY;
      y -= FSA_AVOID_VELOCITY/2;
      hasAvoid = 1;
    }
    if(adcspi_get_output(IR_DISTANCE_ADC_DEVICE, 7) > IR_DISTANCE_AVOID_THRESHOLD) {

      theta += FSA_AVOID_ROTATE_VELOCITY;
      hasAvoid = 1;
    }
    if(adcspi_get_output(IR_DISTANCE_ADC_DEVICE, 8) > IR_DISTANCE_AVOID_THRESHOLD) {
      theta += FSA_AVOID_ROTATE_VELOCITY;
      hasAvoid = 1;
    }
    if(adcspi_get_output(IR_DISTANCE_ADC_DEVICE, 9) > IR_DISTANCE_AVOID_THRESHOLD) {
      x -= FSA_AVOID_VELOCITY/2;
      y -= FSA_AVOID_VELOCITY/2;

      hasAvoid = 1;
    }

    if (!hasAvoid) {
      x = baseX;
      y = baseY;
      theta = baseTheta;
    }

    recharger_act(x, y, theta);
}

void recharger_act(int x, int y, int theta) {
    motor_set_speed_xytheta(x, y, theta);
}


