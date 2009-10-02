/*
 * lib_recharger.h
 *
 * Created by Marc G. Bellemare 
 * Last modified: 27 August 2009
 *
 * This file defines a simple finite state automaton designed to find the
 *  the battery charger and dock to it.
 */

#ifndef LIB_FINDCHARGER_H
#define LIB_FINDCHARGER_H

// Device ID corresponding to the IR distance sensors
#define IR_DISTANCE_ADC_DEVICE      0
#define IR_DISTANCE_SIZE            10
#define IR_LIGHT_ADC_DEVICE         3
#define IR_LIGHT_SIZE               8


#define CF_EVENT_INTERVAL 5
#define CF_WANDER_LENGTH  20
// How many steps to compute the convollution over. This should be even.
#define IR_HISTORY_SIZE   4

#define FSA_WANDER        0
#define FSA_ALIGN         1
#define FSA_FORWARD       2
#define FSA_CLOSE         3
#define FSA_ROTATE_DOCK   4
#define FSA_DONE          5

// Somewhat useless because it doesn't fully imply that the robot will drive
//  in the direction of the target
#define FSA_TARGET_SENSOR 0

#define FSA_FORWARD_VELOCITY      -10
#define FSA_SLOW_FORWARD_VELOCITY -5
#define FSA_ROTATE_VELOCITY       10
#define FSA_SLOW_ROTATE_VELOCITY   5
#define FSA_MAX_ROTATION_COMPONENT 0

#define FSA_AVOID_VELOCITY        (FSA_FORWARD_VELOCITY + 5)
#define FSA_AVOID_ROTATE_VELOCITY (FSA_ROTATE_VELOCITY + 5)

// Threshold below which we assume we don't see the beacon
#define IR_MINIMUM_THRESHOLD    10
// Threshold at which we assume we are near the beacon
#define IR_SATURATION_THRESHOLD 150
#define FSA_GRADIENT_THRESHOLD  30

#define IR_DISTANCE_AVOID_THRESHOLD 150

#define SHORE_POWER_MIN_VOLTAGE     170
// Voltage (in 10ths of volts) at which we trigger the charger finder
#define RECHARGER_LOW_VOLTAGE_TRIGGER 130
// Disable the charger once we reach low_voltage + hysteresis
#define RECHARGER_VOLTAGE_HYSTERESIS   5

/** Event handler for the charger finder. */
int recharger_event( void );
/** Event initialization for the charger finder */
int recharger_init( void );
/** Resets the charger finder to its initial state */
int recharger_reset( void );

/** Enables the FSA for finding the charger. This method does not, on its
  *  own, reset the state of the automaton. */
void recharger_enable( void );
void recharger_disable( void );

/** Takes one step in the FSA */
void recharger_step();

/** Move the robot in the given direction and angle */
void recharger_act(int x, int y, int theta);

/** Move the robot in the given direction and angle, avoid obstacles. */
void recharger_act_avoid(int x, int y, int theta);

/** Update the convolution history used to find the charger */
void recharger_update_history();

/** Enter a new state of the FSA */
void recharger_enter_state(int newState);

/** State methods for the FSA */
void recharger_fsa_wander();
void recharger_fsa_align();
void recharger_fsa_forward();
void recharger_fsa_close();
void recharger_fsa_rotate_dock();
void recharger_fsa_done();

/** Helper methods for computing state information */
int recharger_beacon_dir();
int recharger_sees_beacon();
int recharger_is_moving();
    
/** Drive towards the beacon and wiggle a bit */
void recharger_act_forward(int forwardVel, int rotationVel, 
      int avoidObjects);



#endif /* LIB_FINDCHARGER_H */
