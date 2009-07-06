/*
 * power_avr_utils.c
 *
 *  Created on: Jun 22, 2009
 *      Author: sokolsky
 */

#include "include/pin_names.h"
#include "include/critterbot_power.h"
#include "include/power_avr_charge.h"
#include "include/power_avr_utils.h"
#include <avr/interrupt.h>

void bat40_enable() {
  BATEN_PORT |= BAT40EN;
}

void bat40_disable() {
  BATEN_PORT &= ~BAT40EN;
}

void bat160_enable() {
  BATEN_PORT |= BAT160EN;
}

void bat160_disable() {
  BATEN_PORT &= ~BAT160EN;
}

void bat280_enable() {
  BATEN_PORT |= BAT280EN;
}

void bat280_disable() {
  BATEN_PORT &= ~BAT280EN;
}

void cpu_enable() {
  CPUEN_PORT |= CPUEN;
}

void cpu_disable() {
  CPUEN_PORT &= ~CPUEN;
}

void v3_bus_enable() {
  V3INHIB_PORT &= ~V3INHIB;
}

void v3_bus_disable() {
  V3INHIB_PORT |= V3INHIB;
}

void charger40_disable(void) {
  SHDWN_PORT &= ~SHDWN40;
  SHDWN_DDR |= SHDWN40;
}

void charger40_enable(void) {
  SHDWN_DDR &= ~SHDWN40;
}

void charger160_disable(void) {
  SHDWN_PORT &= ~SHDWN160;
  SHDWN_DDR |= SHDWN160;
}

void charger160_enable(void) {
  SHDWN_DDR &= ~SHDWN160;
}

void charger280_disable(void) {
  SHDWN_PORT &= ~SHDWN280;
  SHDWN_DDR |= SHDWN280;
}

void charger280_enable(void) {
  SHDWN_DDR &= ~SHDWN280;
}

void charger_init(void) {
  charger40_disable();
  charger160_disable();
  charger280_disable();
  charge_read_state();
}

/**
 * returns:
 * 0 - Charging
 * 1 - Invalid state
 * 2 - Top-off charge
 * 3 - Shutdown
 */
uint8_t charger40_status(void) {
  uint8_t retval;

  cli();
  retval = (CHGSTAT_PIN & CHGSTAT40) >> 1;
  CHGSW_PORT |= CHGSW40;
  CHGSW_DDR |= CHGSW40;
  CHGSW_PORT |= CHGSW40;
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  retval += (CHGSTAT_PIN & CHGSTAT40);
  CHGSW_DDR &= ~CHGSW40;
  CHGSW_PORT &= ~CHGSW40;
  sei();
  return retval;
}

uint8_t charger160_status(void) {
  uint8_t retval;

  cli();
  retval = (CHGSTAT_PIN & CHGSTAT160) >> 3;
  CHGSW_PORT |= CHGSW160;
  CHGSW_DDR |= CHGSW160;
  CHGSW_PORT |= CHGSW160;
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  retval += (CHGSTAT_PIN & CHGSTAT160) >> 2;
  CHGSW_DDR &= ~CHGSW160;
  CHGSW_PORT &= ~CHGSW160;
  sei();
  return retval;
}

uint8_t charger280_status(void) {
  uint8_t retval;

  cli();
  retval = (CHGSTAT_PIN & CHGSTAT280) >> 5;
  CHGSW_PORT |= CHGSW280;
  CHGSW_DDR |= CHGSW280;
  CHGSW_PORT |= CHGSW280;
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  __asm__ volatile("nop");
  retval += (CHGSTAT_PIN & CHGSTAT280) >> 4;
  CHGSW_DDR &= ~CHGSW280;
  CHGSW_PORT &= ~CHGSW280;
  sei();
  return retval;
}

// Returns an 8-bit sample from the specified ADC channel.
// This is blocking until the conversion is complete
uint8_t get_adc(uint8_t channel) {
  uint8_t temp;
  ADMUX = 0x20 | channel;
  ADCSRA = 0xC6; // C3 for 1Mhz, C6 for 8Mhz
  while( ADCSRA & 0x40 );
  temp = ADCH;
  return temp;
}

uint8_t get_bat40_voltage(void) {
  static uint8_t avg[ADC_AVG_SAMPLES];
  uint8_t i;
  uint16_t total;

  if(avg[0] == 0) {
    for(i = 0; i < ADC_AVG_SAMPLES; i++)
      avg[i] = get_adc(BAT40SENSE) - 3;
  }
  else {
    for(i = 0; i < ADC_AVG_SAMPLES - 1; i++)
      avg[i] = avg[i+1];
    avg[ADC_AVG_SAMPLES - 1] = get_adc(BAT40SENSE) - 3;
  }
  total = 0;
  for(i = 0; i < ADC_AVG_SAMPLES; i++)
    total += avg[i];
  return (uint8_t)(total / ADC_AVG_SAMPLES);
}

uint8_t get_bat160_voltage(void) {
  static uint8_t avg[ADC_AVG_SAMPLES];
  uint8_t i;
  uint16_t total;

  if(avg[0] == 0) {
    for(i = 0; i < ADC_AVG_SAMPLES; i++)
      avg[i] = get_adc(BAT160SENSE) - 3;
  }
  else {
    for(i = 0; i < ADC_AVG_SAMPLES - 1; i++)
      avg[i] = avg[i+1];
    avg[ADC_AVG_SAMPLES - 1] = get_adc(BAT160SENSE) - 3;
  }
  total = 0;
  for(i = 0; i < ADC_AVG_SAMPLES; i++)
    total += avg[i];
  return (uint8_t)(total / ADC_AVG_SAMPLES);
}

uint8_t get_bat280_voltage(void) {
  static uint8_t avg[ADC_AVG_SAMPLES];
  uint8_t i;
  uint16_t total;

  if(avg[0] == 0) {
    for(i = 0; i < ADC_AVG_SAMPLES; i++)
      avg[i] = get_adc(BAT280SENSE) - 3;
  }
  else {
    for(i = 0; i < ADC_AVG_SAMPLES - 1; i++)
      avg[i] = avg[i+1];
    avg[ADC_AVG_SAMPLES - 1] = get_adc(BAT280SENSE) - 3;
  }
  total = 0;
  for(i = 0; i < ADC_AVG_SAMPLES; i++)
    total += avg[i];
  return (uint8_t)(total / ADC_AVG_SAMPLES);
}

uint8_t get_bat40_current(void) {
  return get_adc(IMON40);
}

uint8_t get_bat160_current(void) {
  return get_adc(IMON280);
}

uint8_t get_bat280_current(void) {
  return get_adc(IMON280);
}

uint8_t get_vsys(void) {
  static uint8_t avg[ADC_AVG_SAMPLES];
  uint8_t i;
  uint16_t total;

  if(avg[0] == 0) {
    for(i = 0; i < ADC_AVG_SAMPLES; i++)
      avg[i] = get_adc(VSYS) - 3;
  }
  else {
    for(i = 0; i < ADC_AVG_SAMPLES - 1; i++)
      avg[i] = avg[i+1];
    avg[ADC_AVG_SAMPLES - 1] = get_adc(VSYS) - 3;
  }
  total = 0;
  for(i = 0; i < ADC_AVG_SAMPLES; i++)
    total += avg[i];
  total = total / ADC_AVG_SAMPLES;

  total = (((total * 69) >> 6) + 1);
  return total;
}

void set_cpu_fan(uint8_t vsys) {
  OCR2B = 30 - (vsys / 10);
}

void cpu_fan_off(void) {
  OCR2B = 0x00;
}

void set_motor_fan(void) {
    OCR1AL = 0xB0;
}

void motor_fan_off(void) {
  OCR1AL = 0x00;
}
