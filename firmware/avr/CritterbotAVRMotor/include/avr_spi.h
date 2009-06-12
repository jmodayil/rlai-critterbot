/*
 * avr_spi.h
 *
 *  Created on: Jun 5, 2009
 *      Author: sokolsky
 */

#ifndef AVR_SPI_H_
#define AVR_SPI_H_

#define SPI_PACKET_HEADER 0x7F
#define SPI_PADDING     0x80

// Initialize SPI device as a bus slave.
void spi_init_slave( void );

#endif /* AVR_SPI_H_ */
