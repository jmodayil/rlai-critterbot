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

#define POWER_CHARGING_DISABLED 0x1

#define CHARGE_VOLTAGE_DELAY 10
#define SYS_LOW_VOLT_THRESH 120
#define CHARGE_THRESH 172

#define CHARGER_COMM_ERROR 0x80
#define CHARGER280_ERROR 0x40
#define CHARGER160_ERROR 0x20
#define CHARGER40_ERROR 0x10
#define FATAL_ERROR 0x08
#define BAT_OK 0x04
#define CHARGE_OK 0x02
#define VOLTAGE_OK 0x01


extern volatile uint8_t system_voltage, rstate;
extern volatile uint8_t commands;
extern uint8_t system_state;
extern uint8_t bat40i, bat160i, bat280i;
extern uint8_t bat40v, bat160v, bat280v;

#endif /* CRITTERBOT_POWER_H_ */
