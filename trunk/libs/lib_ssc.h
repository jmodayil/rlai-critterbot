/*
 *  lib_ssc.h
 *  
 *
 *  Created by Michael Sokolsky on 4/11/08 as lib_spi.h.
 *  Modified by Marc G. Bellemare on 30/4/08 to emulate SPI over the SSC.
 *  Last Revision: 30/4/08
 *
 */

#ifndef LIB_SSC_H
#define LIB_SSC_H
#include "compiler.h"
#include "AT91SAM7S256.h"

#define SSC_INTERRUPT_PRIORITY 1

// Pins that will be assigned to the SSC peripheral
// Unfortunately we don't get a choice here. We ignore RF, TF, RK which are
//  not used by the SPI
#define SSC_A_PINS (AT91C_PA16_TK | AT91C_PA17_TD | AT91C_PA18_RD)

// Delay before a transfer start, in clock cycles
#define SSC_DELAY_BEFORE_TRANSFER 0
// rate = MCK / (value*2); min=1, max=4095 (so baud rates between MCK/2 &
//  MCK/8190)
#define SSC_BAUD_RATE 8
// bits = value; min=2, max=31
#define SSC_WORD_SIZE 12 
// whether to operate in big-endian mode
#define SSC_BIG_ENDIAN 1
// Number of frames (a delay of DELAY_BEFORE_TRANSFER is inserted after
//  each frame)
#define SSC_FRAME_SIZE 0

/*
 * SSC settings
 * Emulates the SPI. As such, FSOS, FSLEN and FSEDGE are ignored - these
 *  are related to frame synchronization
 *
 * Set word size
 * Big-endianess
 * Frame size
 *
 * LOOP/DATDEF, FSOS, FSLEN, FSDEN, FSEDGE ignored
 * @@@ handle FSDEN?
 */
#define SSC_SETTINGS ((SSC_WORD_SIZE-1) & AT91C_SSC_DATLEN) | \
                    ((SSC_BIG_ENDIAN & 0x1) << 7) | \
                    ((SSC_FRAME_SIZE << 8) & AT91C_SSC_DATNB)

typedef struct ssc_packet {
  unsigned int num_words;
  unsigned short *data_to_write;
  unsigned short *read_data;
  unsigned int finished;
  struct ssc_packet *next_packet;
} *ssc_packet_p;
  
// SSC list pointers
struct ssc_packet *ssc_data_head, *ssc_data_tail;

/*
 *  Initialize the SPI unit
 */
void ssc_init();

/*
 *  Add a new packet to the SPI queue
 */
void ssc_send_packet( struct ssc_packet *packet );

/*
 *  SSC Interrupt routine
 *  MAKE SURE THIS IS RUN IN ARM MODE AND IS IN RAM!!!
 *  GCC will likely require different handling than IAR
 *  to accomplish this.
 */
ARM_CODE RAMFUNC void ssc_isr();

#endif /* LIB_SSC_H */
