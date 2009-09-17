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

void led_charge_state();

volatile uint8_t system_voltage, rstate;

/** system_state is a bit-map representing the state of the system, see
  *  critterbot_power.h */
uint8_t system_state;
uint8_t bat40i, bat160i, bat280i;
uint8_t bat40v, bat160v, bat280v;
uint8_t ledCounter;
uint8_t ledBit;

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

  // Enable the dock pullup
  DOCK_EN_DDR |= DOCK_EN;
  DOCK_EN_PORT |= DOCK_EN;

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
  system_state = 0;
}

void fan_init(void) {

  CPUFAN_DDR |= CPUFAN;
  // Pins need to be corrected before using this, unless we want to use Timer1
  MTRFAN_DDR |= MTRFAN;
  // Enable phase-correct PWM, no prescaler, OCR2B output
  TCCR2A = 0x21;
  TCCR2B = 0x09;

  // Temporarily setup so we can cool while charging.
  TCCR1A = 0x81;
  TCCR1B = 0x09;

  // Count up to 0x20
  OCR2A = 0xFF;
  // Initially on until we get a voltage reading
  OCR2B = 0x14;
  OCR1AL = 0xB0;

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
 * otherwise returns 0, with a slight delay and hysteresis;
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
  /* If any of the batteries differ, increment the count (to obtain 
   *  hysteresis) */
  if (diff == 1) {
    if (count < 10)
      count++;
  }
  else {
    if (count > 0)
      count--;
  }
  /* Once we sufficiently often see a difference in voltage, clear the BAT_OK
     flag */
  if (count >= 7) {
    system_state &= ~BAT_OK;
    return 0;
  }
  /* Otherwise set the BAT_OK flag */
  else if (count <= 3) {
    system_state |= BAT_OK;
    return 1;
  }
  else {
    /* Return whether the batteries are okay according to system_state */
    return (system_state & BAT_OK) >> 2;
  }
}

/*
 * returns 1 if system voltage is okay
 * return 0 if it has dropped below SYS_LOW_VOLT_THRESH for 50 cycles.
 * Will only reset to 1 after voltage has risen above 160
 */
int system_voltage_okay(void) {
  static uint8_t count;
  static uint8_t off;

  if (system_voltage > 170)
    off = 0;

  if (system_voltage < SYS_LOW_VOLT_THRESH) {
    if (count < 50)
      count++;
  }
  else
    count = 0;

  if (count == 50 || off == 1) {
    off = 1;
    system_state &= ~VOLTAGE_OK;
    return 0;
  }
  else {
    system_state |= VOLTAGE_OK;
    return 1;
  }
}

/* returns 1 if the voltage rises above CHARGE_THRESH for CHARGE_VOLTAGE_DELAY cycles.
 * returns 0 otherwise
 */
int charge_okay(void) {
  static uint8_t delay;

  if (system_voltage > CHARGE_THRESH) {
    if (delay < CHARGE_VOLTAGE_DELAY)
      delay++;
  }
  else if (delay > 0) {
    delay--;
  }
  if (delay < CHARGE_VOLTAGE_DELAY) {
    system_state &= ~CHARGE_OK;
    return 0;
  }
  else {
    system_state |= CHARGE_OK;
    return 1;
  }
}

uint8_t init_charge_state;

int main(void) {

  uint8_t i;

  charger_init();
  general_init();
  spi_init_slave();
  fan_init();

  init_charge_state = charge_state;

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
  // If we were shutdown and partially charged, we won't continue unless
  // the charger is plugged in!
  if(charge_state != 0 && !(system_state & CHARGE_OK)) {
    motor_fan_off();
    // HA!  Take that state machine!!!
    while(1);
  }

  v3_bus_enable();

  while (1) {
    // Check battery status
    bat40v = get_bat40_voltage();
    bat160v = get_bat160_voltage();
    bat280v = get_bat280_voltage();
    // We don't use these values as of yet, but they will provide a much
    // more accurate time and % estimate of how long it will take for
    // charging to complete, if the time is taken to implement that.
    bat40i = get_bat40_current();
    bat160i = get_bat160_current();
    bat280i = get_bat280_current();
    system_voltage = get_vsys();
    // The following three functions should only be called once per loop,
    // since they have time dependencies in sampling.
    system_voltage_okay();
    battery_level_okay();
    charge_okay();

    switch (system_state) {
    // Voltage is low and batteries unbalanced, can't charge
    // See critterbot_power.h for a system_state breakdown
    case 0: 
    // Unbalanced batteries, can't charge
    case 1: // VOLTAGE_OK
      bat40_disable();
      bat160_disable();
      bat280_disable();
      cpu_disable();
      motor_fan_off();
      cpu_fan_off();
      break;

      // Low battery, can't charge
    case 4: // BAT_OK
      // Suppose batteries should be enabled here.
      bat40_enable();
      bat160_enable();
      bat280_enable();
      cpu_disable();
      cpu_fan_off();
      motor_fan_off();
      break;

      // Normal running mode
    case 5: // BAT_OK, VOLTAGE_OK
      bat40_enable();
      bat160_enable();
      bat280_enable();
      if (!(SW_PIN & SW1)) {
        cpu_enable();
        set_cpu_fan(system_voltage);
      }
      else {
        cpu_disable();
        cpu_fan_off();
      }
      motor_fan_off();
      break;

      // Unbalanced batteries but able to charge
    case 3: // VOLTAGE_OK, CHARGE_OK
      // Balanced batteries and able to charge.
    case 7: // VOLTAGE_OK, CHARGE_OK, BAT_OK
      if (!(SW_PIN & SW1)) {
        cpu_enable();
        set_cpu_fan(system_voltage);
      }
      else {
        cpu_disable();
        cpu_fan_off();
      }
      if(charge_state > 0 && charge_state < 10)
        set_motor_fan();
      else
        motor_fan_off();
      break;

    // Invalid states (because CHARGE_OK occurs only if VOLTAGE_OK)
    case 2: // CHARGE_OK
    case 6: // CHARGE_OK, BAT_OK
    // For now any charge related error will cause us to get here.  An since this shuts off the cpu,
    // we will never get any useful indication of the charge error, this should be remedied in ARM
    // code and the LED displays.
    default:
      bat40_disable();
      bat160_disable();
      bat280_disable();
      cpu_disable();
      // Setting this means we will stay in this state until a reset.
      system_state |= FATAL_ERROR;
      motor_fan_off();
      cpu_fan_off();
      break;
    }

    if (!(SW_PIN & SW2))
      charge();

    // 20ms delay (at 8Mhz)
    _delay_loop_2(40000);

    // Added by MGB: display charge_state
    led_charge_state();
      
  }

  return 0;
}

void led_charge_state() {
  uint8_t max_time = charge_state + 1 + 10;

  if (charge_state == 200) max_time = 25;
  if (system_state & CHARGER_COMM_ERROR) max_time = 30;
  if (system_state & CHARGER40_ERROR) max_time = 35;
  if (system_state & CHARGER160_ERROR) max_time = 40;
  if (system_state & CHARGER280_ERROR) max_time = 45;

  ledCounter++;
  if (ledCounter >= 25) {
    // Display digit 'ledBit'
    ledBit++;
    if (ledBit >= max_time)
      ledBit = 0;

    ledCounter = 0;
  }

  if (ledBit >= 0 && ledBit < 5)
    LED1_PORT |= LED1;
  else if (ledBit >= 5 && ledBit < 10)
    LED1_PORT &= ~LED1;
  else if (ledCounter >= 12) {
    // Turn the LED off for half a cycle
    LED1_PORT &= ~LED1;
  }
  else
    LED1_PORT |= LED1;
}
