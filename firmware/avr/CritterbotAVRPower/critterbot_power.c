/*
 * critterbot_power.c
 *
 *  Created on: Jun 5, 2009
 *      Author: sokolsky
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "include/pin_names.h"

#define SPI_PACKET_HEADER 0x7F
#define SPI_PADDING 0x80

uint8_t sys_voltage, rstate, dummy;

/*
 * SPI interrupt routine
 */
ISR(SPI_STC_vect) {

  switch(rstate) {
    case 0:
      dummy = (uint8_t)SPDR;
      SPDR = sys_voltage;
      if(dummy == SPI_PACKET_HEADER)
        rstate = 1;
      else
        rstate = 0;
      break;
    case 1:
      dummy = (uint8_t)SPDR;
      SPDR = SPI_PADDING;
      rstate = 2;
      break;
    case 2:
      dummy = (uint8_t)SPDR;
      SPDR = 0;
      rstate = 0;
      break;
    default:
      dummy = (uint8_t)SPDR;
      SPDR = 0;
      rstate = 0;
      break;

  }
}


/*
 *  This reports system Voltage as:
 *  System Voltage = (val + 148) * 7 / 108
 *  or Approx:
 *  System Voltage = (val + 144) / 15
 *  This is close to linear, above 17V it starts to roll off
 *  because of the protection diode.
 */
uint8_t get_sys_volt( void ) {

  uint16_t raw;
  uint8_t val;
  raw = ADCL;
  raw += (((uint16_t)ADCH) << 8);
  ADMUX = 0x02;
  ADCSRA = 0xC3;
  val = (raw - 300) >> 1;
  return val;
}


void adc_init( void ) {

  ADMUX = 0x00;
  ADCSRA = 0xC3; // C3 for 1Mhz, C6 for 8Mhz
  while( ADCSRA & 0x40 );
}


void fan_init(void) {

  CPUFAN_DDR |= CPUFAN_PIN;
  //DDRD |= 0x60;
  TCCR2A = 0x21;
  TCCR0B = 0x09;
  OCR2A = 0x20;
  OCR2B = 0x10;
  //OCR0A = 0x20;
  //OCR0B = 0x01;
  //TCCR0A = 0x21;
  //TCCR0B = 0x09;

}

void spi_init_slave( void ) {

  cli();
  DDRB = 0x10;
  SPCR = _BV(SPE)|_BV(CPHA)|_BV(SPIE);
  SPDR = 0x00;
  sei();
}

int main(void) {

  volatile uint16_t i;
  unsigned char dat;
  // Clear everything for safety sake
  PORTB = 0x00;
  PORTC = 0x00;
  PORTD = 0x00;

  DDRC |= 0x03;
  DDRD |= 0xAC;

  PORTC |= 0x01;
  PORTD |= 0x80;


  spi_init_slave();
  rstate = 0;
  fan_init();

  while(1) {
    for(i = 0; i < 10000; i++);
    sys_voltage = get_sys_volt();
    dat = 10 - (sys_voltage / 11);
    //OCR0B = dat;

  }

}
