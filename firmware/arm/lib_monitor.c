/*
 *  lib_monitor.h
 *  
 *
 *  Created by Marc G. Bellemare on 01/10/10
 *
 *  A monitor driving various failsafes on the ARM: finding the charger,
 *   disabling motor commands if the motor temperatures fall too low, etc.
 */

#include "lib_motor.h"
#include "lib_monitor.h"
#include "lib_events.h"

event_s monitor_event_s = {
  monitor_init,
  monitor_event,
  0
};

int monitor_bad_motor_temp = 0;

int monitor_init() {
  return 0;
}

int monitor_event() {
  // The method calls here should be in increasing order of priority.
  // Verify the battery voltage
  monitor_battery_charge();
  // Verify the motor temperature.
  monitor_motor_temp();

  return 1;
}

int monitor_battery_charge() {
  int voltage = motor_get_voltage();

  return 0;
}

int monitor_motor_temp() {
  int i, temp;
  int bad_temp = 0;
  int good_temp = 0;

  // Poll the motor temperature
  for (i = 0; i < MOTOR_NUM_MOTORS; i++) {
    temp = motor_temp(i);
    if (temp < MONITOR_MIN_MOTOR_TEMPERATURE) {
      bad_temp = 1;
      break;
    }
    else if (temp >= 
      MONITOR_MIN_MOTOR_TEMPERATURE + MONITOR_TEMPERATURE_HYSTERESIS) {
      good_temp++;
    }
  }

  // If we were in a bad state but now all three motors are good, 
  //  unparalyze the robot
  if (monitor_bad_motor_temp == 1 && good_temp == 3) {
    motor_enable_drive();
    monitor_bad_motor_temp = 0;
  }
  // If any of the motors are overheated, shut down the system
  if (bad_temp) {
    monitor_bad_motor_temp = 1;
  }

  // Let's make sure the motors are paralyzed if the temperature is too 
  //  low
  if (monitor_bad_motor_temp) {
    motor_disable_drive();
    return 1;
  }
  else {
    return 0;
  }
}

