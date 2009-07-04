/*
 * critterbot_power.h
 *
 *  Created on: Jun 22, 2009
 *      Author: sokolsky
 */

#ifndef CRITTERBOT_POWER_H_
#define CRITTERBOT_POWER_H_

#define SPI_PACKET_HEADER 0x7A
#define SPI_PADDING 0x7D
#define CHARGE_VOLTAGE_DELAY 10

#define BAT_OK 0x04
#define CHARGE_OK 0x02
#define VOLTAGE_OK 0x01


extern volatile uint8_t system_voltage, rstate;
extern uint8_t system_state;
extern uint8_t bat40i, bat160i, bat280i;
extern uint8_t bat40v, bat160v, bat280v;

#endif /* CRITTERBOT_POWER_H_ */
