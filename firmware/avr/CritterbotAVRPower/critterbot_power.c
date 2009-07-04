/*
 * critterbot_power.c
 *
 *  Created on: Jun 5, 2009
 *      Author: sokolsky
 */

#include "include/pin_names.h"
#include "include/critterbot_power.h"
#include "include/power_avr_utils.h"
#include "include/power_avr_charge.h"
#include <avr/interrupt.h>
#include <util/delay_basic.h>

volatile uint8_t system_voltage, rstate;

uint8_t system_state;
uint8_t bat40i, bat160i, bat280i;
uint8_t bat40v, bat160v, bat280v;

ISR(SPI_STC_vect)
{
  uint8_t dummy;

  switch (rstate) {
  case 0:
    dummy = (uint8_t) SPDR;
    SPDR = system_voltage;
    if (dummy == SPI_PACKET_HEADER)
      rstate = 1;
    else
      rstate = 0;
    break;
  case 1:
    dummy = (uint8_t) SPDR;
    SPDR = charge_state;
    rstate = 2;
    break;
  case 2:
    dummy = (uint8_t) SPDR;
    SPDR = bat40v;
    rstate = 3;
    break;
  case 3:
    dummy = (uint8_t) SPDR;
    SPDR = bat160v;
    rstate = 4;
    break;
  case 4:
    dummy = (uint8_t) SPDR;
    SPDR = bat280v;
    rstate = 5;
    break;
  case 5:
    dummy = (uint8_t) SPDR;
    SPDR = SPI_PADDING;
    rstate = 0;
    break;
  default:
    dummy = (uint8_t) SPDR;
    SPDR = 0;
    rstate = 0;
    break;

  }
}

void general_init(void) {
  // BATEN are output and default off
  BATEN_DDR |= (BAT280EN | BAT160EN | BAT40EN);
  // CPUEN is output and defaults off
  CPUEN_DDR |= CPUEN;
  // V3INHIB is output and defaults off (inverted control)
  V3INHIB_PORT |= V3INHIB;
  V3INHIB_DDR |= V3INHIB;

  // Pullups on for switch input
  SW_PORT |= (SW1 | SW2);
  // LED1 is output
  LED1_DDR |= LED1;

  // Reset the motor controllers since they don't seem to work right on
  // a fresh power-up
  AVRRESET_PORT |= AVRRESET;
  // AVRRESET is output
  AVRRESET_DDR |= AVRRESET;
  AVRRESET_PORT |= AVRRESET;
  _delay_loop_2(60000);
  AVRRESET_PORT &= ~AVRRESET;
  _delay_loop_2(30000);
  AVRRESET_DDR &= ~AVRRESET;
  AVRRESET_PORT &= ~AVRRESET;

  // LED1 on
  //LED1_PORT |= LED1;
  // AVRRESET high
  //AVRRESET_PORT |= AVRRESET_PIN;
}

void fan_init(void) {

  CPUFAN_DDR |= CPUFAN;
  // Pins need to be corrected before using this, unless we want to use Timer1
  MTRFAN_DDR |= MTRFAN;
  // Enable phase-correct PWM, no prescaler, OCR2B output
  TCCR2A = 0x21;
  TCCR2B = 0x09;
  // Count up to 0x20
  OCR2A = 0x20;
  // Initially full output until we get a voltage reading
  OCR2B = 0x20;

}

void adc_init(void) {
  ADMUX = 0x20; // Left adjust
  ADCSRA = 0xC6; // C3 for 1Mhz, C6 for 8Mhz
  while (ADCSRA & 0x40)
    ;
}

void spi_init_slave(void) {

  cli();
  SPI_DDR |= MISO;
  SPCR = _BV(SPE) | _BV(CPHA) | _BV(SPIE);
  SPDR = 0x00;
  sei();
}

/*
 * Returns 1 if the battery levels are acceptably close
 * otherwise returns 0, with a slight delay;
 */
int battery_level_okay(void) {
  static uint8_t count;
  uint8_t diff = 0;

  if (bat40v > bat160v + 2)
    diff = 1;
  if (bat160v > bat40v + 2)
    diff = 1;
  if (bat40v > bat280v + 2)
    diff = 1;
  if (bat280v > bat40v + 2)
    diff = 1;
  if (bat160v > bat280v + 2)
    diff = 1;
  if (bat280v > bat160v + 2)
    diff = 1;
  if (diff == 1) {
    if (count < 10)
      count++;
  } else {
    if (count > 0)
      count--;
  }
  if (count >= 5) {
    system_state &= ~BAT_OK;
    return 0;
  } else {
    system_state |= BAT_OK;
    return 1;
  }
}

/*
 * returns 1 if system voltage is okay.../critterbot_power.c:98: error: ‘battery_state’ undeclared (first use in this function)
 *
 * return 0 if it has dropped too low.
 * Will only reset to 1 after voltage has risen above 160
 */
int system_voltage_okay(void) {
  static uint8_t count;
  static uint8_t off;

  if (system_voltage > 170)
    off = 0;
  if (system_voltage < 120) {
    if (count < 50)
      count++;
  } else
    count = 0;
  if (count == 50 || off == 1) {
    off = 1;
    system_state &= ~VOLTAGE_OK;
    return 0;
  } else {
    system_state |= VOLTAGE_OK;
    return 1;
  }
}

/* returns 1 if we're sufficiently convinced the charger is attached.
 * returns 0 otherwise
 */
int charge_okay(void) {
  static uint8_t delay;

  if (system_voltage > 172) {
    if (delay < CHARGE_VOLTAGE_DELAY)
      delay++;
  } else if (delay > 0)
    delay--;
  if (delay < CHARGE_VOLTAGE_DELAY) {
    system_state &= ~CHARGE_OK;
    return 0;
  }
  else {
    system_state |= CHARGE_OK;
    return 1;
  }
}

int main(void) {

  uint8_t i;

  charger_init();
  general_init();
  spi_init_slave();

  for (i = 0; i < 55; i++) {
    // Get a sufficient sample of battery and system voltages to begin with
    bat40v = get_bat40_voltage();
    bat160v = get_bat160_voltage();
    bat280v = get_bat280_voltage();
    battery_level_okay();
    system_voltage = get_vsys();
    system_voltage_okay();
    charge_okay();
    // 20ms delay (at 8Mhz)
    _delay_loop_2(40000);
  }

  fan_init();

  // Check if the battery levels are okay, if yes, then we can startup.
  if ((system_state & BAT_OK) && (system_state & VOLTAGE_OK)) {
    bat40_enable();
    bat160_enable();
    bat280_enable();
  }

  while (1) {
    // Check battery status
    bat40v = get_bat40_voltage();
    bat160v = get_bat160_voltage();
    bat280v = get_bat280_voltage();
    bat40i = get_bat40_current();
    bat160i = get_bat160_current();
    bat280i = get_bat280_current();
    system_voltage = get_vsys();
    system_voltage_okay();
    battery_level_okay();

    switch(system_state) {
    case 1:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 7:
      break;
    // Voltage is low and uneven
    case 0:
    // Invalid states
    case 2:
    case 6:
    default:
      break;
    }
    // Disable batteries if they are not at equal charge levels
    // THIS DOES NOT WORK DUMMY!
    if (!battery_level_okay()) {
      //bat40_disable();
      //bat160_disable();
      //bat280_disable();
    } else {
      // Some sane way of recovering?
    }
    // Check system voltage and adjust fan speeds
    set_cpu_fan(system_voltage);
    set_motor_fan(system_voltage);

    // Disable things if system voltage gets too low
    if (!system_voltage_okay()) {
      LED1_PORT |= LED1;
      v3_bus_disable();
      cpu_disable();
      // Doing the following can't hurt in this situation.  But is this really what we
      // want to do?
      bat40_disable();
      bat160_disable();
      bat280_disable();
    } else {
      v3_bus_enable();
      // Only enable the CPU is SW1 is set
      if (!(SW_PIN & SW1))
        cpu_enable();
      else
        cpu_disable();
    }

    // Enable the charger if SW2 is set
    if (!(SW_PIN & SW2)) {
      bat40_disable();
      bat160_disable();
      bat280_disable();
      charge();
      // Charge error!!!
      if (charge_state >= 200) {
        // What to do??!?!
      }
    } else {
      //if(battery_level_okay()) {
      //  bat40_enable();
      //  bat160_enable();
      //  bat280_enable();
      //}
      charger40_disable();
      charger160_disable();
      charger280_disable();
      //charge_state = 0;
    }

    // 20ms delay (at 8Mhz)
    _delay_loop_2(40000);
  }

  return 0;
}
