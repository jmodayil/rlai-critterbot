/*
 * power_avr_charge.h
 *
 *  Created on: Jun 22, 2009
 *      Author: sokolsky
 */

#ifndef POWER_AVR_CHARGE_H_
#define POWER_AVR_CHARGE_H_

#define CHARGE_VOLTAGE_DELAY 10

extern uint8_t charge_state;

void charge( void );

#endif /* POWER_AVR_CHARGE_H_ */
