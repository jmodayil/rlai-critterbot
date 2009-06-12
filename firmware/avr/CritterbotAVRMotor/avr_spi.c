/*
 * avr_spi.c
 *
 *  Created on: Jun 5, 2009
 *      Author: sokolsky
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "include/avr_spi.h"

void spi_init_slave( void ) {

  cli();
  DDRB = 0x10;
  SPCR = _BV(SPE)|_BV(CPHA)|_BV(SPIE);
  SPDR = 0;
  sei();
}
