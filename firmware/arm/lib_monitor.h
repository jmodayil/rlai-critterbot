/*
 *  lib_monitor.h
 *  
 *
 *  Created by Marc G. Bellemare on 01/10/10
 *
 *  A monitor driving various failsafes on the ARM: finding the charger,
 *   disabling motor commands if the motor temperatures fall too low, etc.
 */
#ifndef LIB_MONITOR_H
#define LIB_MONITOR_H

#define MONITOR_STATUS_MOTOR_TEMP   0x1

// #define MONITOR_MIN_MOTOR_TEMPERATURE 65
// For testing purposes only
#define MONITOR_MIN_MOTOR_TEMPERATURE 65
#define MONITOR_TEMPERATURE_HYSTERESIS 20
#define MONITOR_INITIAL_BURN_IN_LENGTH 100

int monitor_init();
int monitor_event();

int monitor_motor_temp();
int monitor_battery_charge();

/** Returns the status of the monitor. */
int monitor_status();

#endif /* LIB_MONITOR_H */
