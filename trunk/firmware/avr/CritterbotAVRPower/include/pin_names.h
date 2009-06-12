/*
 * PinNames.h
 *
 *  Created on: Jun 4, 2009
 *      Author: sokolsky
 */

#ifndef PIN_NAMES_H_
#define PIN_NAMES_H_

#include <avr/io.h>

// Port B connections
#define SPI_PORT        PORTB
#define SPI_DDR         DDRB
#define SPI_PIN         PINB
#define SPISCK          0x80
#define MISO            0x40
#define MOSI            0x20
#define SPICS           0x10
#define LED1            0x08
#define LED1_PORT       PORTB
#define LED1_PIN        PINB
#define LED1_DDR        DDRB
#define SHDWN_PORT      PORTB
#define SHDWN_DDR       DDRB
#define SHDWN_PIN       PINB
#define SHDWN280        0x04
#define SHDWN160        0x02
#define SHDWN40         0x01

// Port C connections
#define SW1             0x80
#define SW2             0x40
#define SW_PORT         PORTC
#define SW_DDR          DDRC
#define SW_PIN          PINC
#define CHGSTAT_PORT    PORTC
#define CHGSTAT_DDR     DDRC
#define CHGSTAT_PIN     PINC
#define CHGSW_PORT      PORTC
#define CHGSW_DDR       DDRC
#define CHGSW_PIN       PINC
#define CHGSTAT280      0x20
#define CHGSW280        0x10
#define CHGSTAT160      0x08
#define CHGSW160        0x04
#define CHGSTAT40       0x02
#define CHGSW40         0x01

// Port D connections
#define AVRRESET_PORT   PORTD
#define AVRRESET_DDR    DDRD
#define AVRRESET_PIN    PIND
#define AVRRESET        0x80
#define CPUFAN_PORT     PORTD
#define CPUFAN_DDR      DDRD
#define CPUFAN_PIN      PIND
#define CPUFAN          0x40
#define MTRFAN_PORT     PORTD
#define MTRFAN_DDR      DDRD
#define MTRFAN_PIN      PIND
#define MTRFAN          0x20
#define V3INHIB_PORT    PORTD
#define V3INHIB_DDR     DDRD
#define V3INHIB_PIN     PIND
#define V3INHIB         0x10
#define CPUEN_PORT      PORTD
#define CPUEN_DDR       DDRD
#define CPUEN_PIN       PIND
#define CPUEN           0x08
#define BATEN_PORT      PORTD
#define BATEN_DDR       DDRD
#define BATEN_PIN       PIND
#define BAT280EN        0x04
#define BAT160EN        0x02
#define BAT40EN         0x01

#endif /* PIN_NAMES_H_ */
