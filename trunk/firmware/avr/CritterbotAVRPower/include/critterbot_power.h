/*
 * critterbot_power.h
 *
 *  Created on: Jun 22, 2009
 *      Author: sokolsky
 */

#ifndef CRITTERBOT_POWER_H_
#define CRITTERBOT_POWER_H_

#define SPI_PACKET_HEADER 0x7F
#define SPI_PADDING 0x80

extern volatile uint8_t system_voltage, rstate;
extern uint8_t bat40i, bat160i, bat280i;
extern uint8_t bat40v, bat160v, bat280v;

#endif /* CRITTERBOT_POWER_H_ */
