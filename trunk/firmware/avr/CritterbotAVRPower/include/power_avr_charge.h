/*
 * power_avr_charge.h
 *
 *  Created on: Jun 22, 2009
 *      Author: sokolsky
 */

#ifndef POWER_AVR_CHARGE_H_
#define POWER_AVR_CHARGE_H_

#define MIN_BAT_CHARGE_VOLTAGE 155

#define CHARGE_STAT_EEPROM ((uint8_t*)0x01)

extern uint8_t charge_state;
extern uint8_t saved_charge_state;

void set_charge_state(uint8_t new_state);
void read_charge_state(void);
void charge( void );

#endif /* POWER_AVR_CHARGE_H_ */
