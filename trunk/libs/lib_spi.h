/*
 *  lib_spi.h
 *  
 *
 *  Created by Michael Sokolsky on 4/11/08.
 *  Last Revision: 01/07/08
 *
 */

#include "AT91SAM7S256.h"
#include "compiler.h"

// Delay between CS signal changes = value / MCK
// Min = 6, Max = 255
#define DELAY_BETWEEN_CS (6 << 24)

#define SPI_INTERRUPT_PRIORITY 1

// Pins that will be assigned to the SPI peripheral
#define SPI_A_PINS  ( AT91C_PA14_SPCK | AT91C_PA13_MOSI | \
                      AT91C_PA12_MISO | AT91C_PA11_NPCS0 | AT91C_PA31_NPCS1 )
#define SPI_B_PINS  ( AT91C_PA10_NPCS2 | AT91C_PA22_NPCS3 )

// Transfer settings for chip selects 0 - 3
// delay = 32 * value / MCK; min=0, max=255
#define SPI_CS0_3_DELAY_BETWEEN_TRANSFERS 0
// delay = value / MCK; min=0, max=255
#define SPI_CS0_3_DELAY_BEFORE_CLOCK 0
// rate = MCK / value; min=1, max=255
#define SPI_CS0_3_BAUD_RATE 1
// bits = 8 + value; min=0, max=8
#define SPI_CSO_3_WORD_SIZE 0x0
// bit 0: Clock polarity, 0 clock starts low, 1 clock starts high
// bit 1: Clock phase, 0 leading edge change, 1 leading edge capture
// bit 2: Don't care
// bit 3: Chip select behavior, 0 nCS rises after transfer, 1 stays low
#define SPI_CSO_3_FLAGS 0x0
#define SPI_CSR0_SETTINGS ( SPI_CS0_3_DELAY_BETWEEN_TRANSFERS<<24 | \
                              SPI_CS0_3_DELAY_BEFORE_CLOCK<<16 | \
                              SPI_CS0_3_BAUD_RATE<<8 | \
                              SPI_CSO_3_WORD_SIZE<<4 | \
                              SPI_CSO_3_FLAGS )


// Transfer settings for chip selects 4 - 7
// delay = 32 * value / MCK; min=0, max=255
#define SPI_CS4_7_DELAY_BETWEEN_TRANSFERS 0
// delay = value / MCK; min=0, max=255
#define SPI_CS4_7_DELAY_BEFORE_CLOCK 0
// rate = MCK / value; min=1, max=255
#define SPI_CS4_7_BAUD_RATE 1
// bits = 8 + value; min=0, max=8
#define SPI_CS4_7_WORD_SIZE 0x0
// bit 0: Clock polarity, 0 clock starts low, 1 clock starts high
// bit 1: Clock phase, 0 leading edge change, 1 leading edge capture
// bit 2: Don't care
// bit 3: Chip select behavior, 0 nCS rises after transfer, 1 stays low
#define SPI_CS4_7_FLAGS 0x0
#define SPI_CSR1_SETTINGS ( SPI_CS4_7_DELAY_BETWEEN_TRANSFERS<<24 | \
                              SPI_CS4_7_DELAY_BEFORE_CLOCK<<16 | \
                              SPI_CS4_7_BAUD_RATE<<8 | \
                              SPI_CS4_7_WORD_SIZE<<4 | \
                              SPI_CS4_7_FLAGS )


// Transfer settings for chip selects 8 - 11
// delay = 32 * value / MCK; min=0, max=255
#define SPI_CS8_11_DELAY_BETWEEN_TRANSFERS 0
// delay = value / MCK; min=0, max=255
#define SPI_CS8_11_DELAY_BEFORE_CLOCK 0
// rate = MCK / value; min=1, max=255
#define SPI_CS8_11_BAUD_RATE 64
// bits = 8 + value; min=0, max=8
#define SPI_CS8_11_WORD_SIZE 0x0
// bit 0: Clock polarity, 0 clock starts low, 1 clock starts high
// bit 1: Clock phase, 0 leading edge change, 1 leading edge capture
// bit 2: Don't care
// bit 3: Chip select behavior, 0 nCS rises after transfer, 1 stays low
#define SPI_CS8_11_FLAGS 0x3
#define SPI_CSR2_SETTINGS ( SPI_CS8_11_DELAY_BETWEEN_TRANSFERS<<24 | \
                              SPI_CS8_11_DELAY_BEFORE_CLOCK<<16 | \
                              SPI_CS8_11_BAUD_RATE<<8 | \
                              SPI_CS8_11_WORD_SIZE<<4 | \
                              SPI_CS8_11_FLAGS )


// Transfer settings for chip selects 12 - 14
// delay = 32 * value / MCK; min=0, max=255
#define SPI_CS12_14_DELAY_BETWEEN_TRANSFERS 0
// delay = value / MCK; min=0, max=255
#define SPI_CS12_14_DELAY_BEFORE_CLOCK 0
// rate = MCK / value; min=1, max=255
#define SPI_CS12_14_BAUD_RATE 1
// bits = 8 + value; min=0, max=8
#define SPI_CS12_14_WORD_SIZE 0x0
// bit 0: Clock polarity, 0 clock starts low, 1 clock starts high
// bit 1: Clock phase, 0 leading edge change, 1 leading edge capture
// bit 2: Don't care
// bit 3: Chip select behavior, 0 nCS rises after transfer, 1 stays low
#define SPI_CS12_14_FLAGS 0x0
#define SPI_CSR3_SETTINGS ( SPI_CS12_14_DELAY_BETWEEN_TRANSFERS<<24 | \
                              SPI_CS12_14_DELAY_BEFORE_CLOCK<<16 | \
                              SPI_CS12_14_BAUD_RATE<<8 | \
                              SPI_CS12_14_WORD_SIZE<<4 | \
                              SPI_CS12_14_FLAGS )

/*
 *  Holds information about a single packet for SPI transmission
 *
 *  device_id = chip select value
 *  num_words = number of words in this tranmission
 *  data_to_write = pointer to the data to send, cannot be null
 *  read_data = pointer to write read data to, if null, no read data will be 
 *    saved, if non-null, have at least num_words allocated
 *    it is safe to set this to the same address as data_to_write, write
 *    operations occur before read
 *  finished = this is incremented every time new data is read from the SPI
 *  spi_packet = Internal reference.  Do not change.
 */
typedef struct spi_packet {
  unsigned int device_id;
  unsigned int num_words;
  unsigned int* data_to_write;
  unsigned int* read_data;
  volatile unsigned int finished;
  struct spi_packet *next_packet;
} * spi_packet_p;

// SPI list pointers
struct spi_packet *spi_data_head, *spi_data_tail;

/*
 *  Initialize the SPI unit
 */
void spi_init();

/*
 *  Add a new packet to the SPI queue
 */
void spi_send_packet( struct spi_packet *packet );

/*
 *  SPI Interrupt routine
 *  MAKE SURE THIS IS RUN IN ARM MODE AND IS IN RAM!!!
 *  GCC will likely require different handling than IAR
 *  to accomplish this.
 */
ARM_CODE RAMFUNC void spi_isr();
