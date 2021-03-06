/*
 * lib_power.h
 *
 * Created by Marc G. Bellemare 
 * Last modified: 28 August 09
 */

#ifndef LIB_POWER_H
#define LIB_POWER_H

#define POWER_CHARGE_NOT_CHARGING  0
#define POWER_CHARGE_40            2
#define POWER_CHARGE_40_160        4
#define POWER_CHARGE_160           5
#define POWER_CHARGE_160_280       7
#define POWER_CHARGE_280           8
#define POWER_CHARGE_TOPOFF        9
#define POWER_CHARGE_COMPLETE      11
#define POWER_CHARGE_ERROR         200 

// A flag sent to the AVR to indicate that we should not charge
#define POWER_CHARGING_DISABLED    0x01

#endif /* LIB_POWER_H */

