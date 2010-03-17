/*
 * power_avr_charge.h
 *
 *  Created on: Jun 22, 2009
 *      Author: sokolsky
 */

#ifndef POWER_AVR_CHARGE_H_
#define POWER_AVR_CHARGE_H_

// Don't charge unless the batteries
// are below this voltage
// Changed from 15.5V to 16V by MGB due to the addition of the software switch
#define MIN_BAT_CHARGE_VOLTAGE 160
// If the voltage falls below 14V, enable charge irrespective of the disabled
//  flag
#define OVERRIDE_CHARGING_DISABLED_VOLTAGE 140
// Don't charge unless the batteries
// are above this voltage
#define MIN_BAT_VOLTAGE 80

#define CHARGE_STAT_EEPROM ((uint8_t*)0x01)

extern uint8_t charge_state;
extern uint8_t saved_charge_state;

void set_charge_state(uint8_t new_state);
void read_charge_state(void);
void charge( void );

#endif /* POWER_AVR_CHARGE_H_ */
