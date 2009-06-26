/*
 * power_avr_utils.h
 *
 *  Created on: Jun 22, 2009
 *      Author: sokolsky
 */

#ifndef POWER_AVR_UTILS_H_
#define POWER_AVR_UTILS_H_

#define VSYS 0
#define BAT40SENSE 1
#define BAT160SENSE 2
#define BAT280SENSE 3
#define IMON40 4
#define IMON160 5
#define IMON280 6

void bat40_enable();
void bat40_disable();
void bat160_enable();
void bat160_disable();
void bat280_enable();
void bat280_disable();

void cpu_enable();
void cpu_disable();

void v3_bus_enable();
void v3_bus_disable();

void charger40_disable(void);
void charger40_enable(void);
void charger160_disable(void);
void charger160_enable(void);
void charger280_disable(void);
void charger280_enable(void);

void charger_init(void);

/**
 * returns:
 * 0 - Charging
 * 1 - Invalid state
 * 2 - Top-off charge
 * 3 - Shutdown
 */
uint8_t charger40_status(void);
uint8_t charger160_status(void);
uint8_t charger280_status(void);

uint8_t get_adc(uint8_t channel);
uint8_t get_bat40_voltage(void);
uint8_t get_bat160_voltage(void);
uint8_t get_bat280_voltage(void);
uint8_t get_bat40_current(void);
uint8_t get_bat160_current(void);
uint8_t get_bat280_current(void);
uint8_t get_vsys(void);

void set_cpu_fan(uint8_t vsys);
void set_motor_fan(uint8_t vsys);


#endif /* POWER_AVR_UTILS_H_ */
