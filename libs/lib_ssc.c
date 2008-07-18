/*
 *  lib_ssc.c
 *  
 *
 *  Created by Michael Sokolsky on 4/11/08 as lib_spi.c.
 *  Modified by Marc G. Bellemare on 30/04/08 to emulate the SPI over the SSC.
 *  Last Revision: 30/04/08
 *
 */

#include "compiler.h"
#include "AT91SAM7S256.h"
#include "lib_AT91SAM7S256.h"
#include "lib_ssc.h"
#include <stdio.h>

#include "armio.h"
/*
 *  Initialize the SSC unit
 */
void ssc_init() {
  
  AT91PS_SSC ssc = AT91C_BASE_SSC;

  ssc_data_head = ssc_data_tail = NULL;
  *AT91C_PMC_PCER = 1 << AT91C_ID_SSC;

  // Configure the PIO pins
  *AT91C_PIOA_ASR = SSC_A_PINS;
  *AT91C_PIOA_PDR = SSC_A_PINS;

  // Set the SSC baud rate
  ssc->SSC_CMR = (SSC_BAUD_RATE & 0xFFF);

  // @@@ design decision - RK as an input or as an output + floating pin?

  /*
   * SSC Receive Clock Mode Register
   * Set parameters for the receive clock. 
   *
   *  Clock tied to transmit clock (SPI emulation)
   *  RK pin is an output (unused)
   *  Receive clock on only during transfers (unused)
   *  Continuous receiving mode (tie receiving solely to receive clock)
   *  Delay of SSC_DELAY_BEFORE_TRANSFER between start & receive/transmit
   *  Receive clock gating (CKG) = 0
   *  PERIOD = 0 (no sync signal)
   */
  ssc->SSC_RCMR = ( AT91C_SSC_CKS_TK | 
                    AT91C_SSC_CKO_NONE |
                    AT91C_SSC_CKI |
                    AT91C_SSC_START_CONTINOUS | // [sic]!
                    ((SSC_DELAY_BEFORE_TRANSFER << 16) & AT91C_SSC_STTDLY)
                  );
  
   /* SSC Transmit Clock Mode Register
   * Set parameters for the transmit clock. 
   *
   *  Clock is the divided clock (as per SSC_BAUD_RATE)
   *  TK pin is an output
   *  Transmit clock only during transfer
   *  Delay of SSC_DELAY_BEFORE_TRANSFER between start & receive/transmit
   *  Transmit clock gating (CKG) = 0
   *  PERIOD = 0 (no sync signal)
   */
  ssc->SSC_TCMR = ( AT91C_SSC_CKS_DIV | 
                    AT91C_SSC_CKO_DATA_TX |
                    AT91C_SSC_START_CONTINOUS | // [sic] also!
                    ((SSC_DELAY_BEFORE_TRANSFER << 16) & AT91C_SSC_STTDLY)
                  );
  // @@@ some error checking - wordsize > 1, PDC size depends on DATLEN
  /*
   * SSC Receive Frame Mode Register, Transmit Frame Mode Register
   * Set parametsrs for the receive and transmit frames.
   *
   * Word size
   * Big-endianess
   * # of words per frame (note: a delay of DELAY_BEFORE_TRANSFER is inserted
   *  between each frame)
   *
   * The TFMR also has a DATDEF field, which we set to 0
   */
  ssc->SSC_RFMR = SSC_SETTINGS;
  ssc->SSC_TFMR = SSC_SETTINGS;
  // Enable an interrupt on end of transfer/receive
  AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC,
                          AT91C_ID_SSC,
                          SSC_INTERRUPT_PRIORITY,
                          AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE,
                          ssc_isr);
  ssc->SSC_IER = AT91C_SSC_ENDTX | AT91C_SSC_ENDRX;
  AT91F_AIC_EnableIt ( AT91C_BASE_AIC, AT91C_ID_SSC );
  // Enable SSC
  ssc->SSC_CR = AT91C_SSC_RXEN | AT91C_SSC_TXEN;
}


/*
 *  Add a new packet to the SSC transmission queue
 * Copied from spi_send_packet
 */
void ssc_send_packet( struct ssc_packet *packet ) {
  
  AT91PS_SSC ssc = AT91C_BASE_SSC;

  // This is the last packet in the list.
  packet->next_packet = NULL;
  // If this isn't also the first packet in the list, add it to 
  // the end
  if( ssc_data_head != NULL ) {
    ssc_data_head->next_packet = packet;
    ssc_data_head = packet;
  }
  // If this is the first packet in the list, set up the PDC and
  // start the transfer
  else {
    ssc_data_tail = ssc_data_head = packet;
    ssc->SSC_TPR = (AT91_REG)ssc_data_tail->data_to_write;
    ssc->SSC_RPR = (AT91_REG)ssc_data_tail->read_data;
    // If read_data is NULL, we're ignoring received data
    if( ssc_data_tail->read_data != NULL )
    {
      ssc->SSC_RCR = ssc_data_tail->num_words;
      ssc->SSC_CR = AT91C_SSC_RXEN;
      ssc->SSC_PTCR = AT91C_PDC_RXTEN;
    }
    else
    {
      ssc->SSC_RCR = 0;
      // Disable RX if not reading anything
      ssc->SSC_CR = AT91C_SSC_RXDIS;
      ssc->SSC_PTCR = AT91C_PDC_RXTDIS;
    }
    ssc->SSC_TCR = ssc_data_tail->num_words;
    ssc->SSC_PTCR = AT91C_PDC_TXTEN;
  }
  
}


/*
 *  SSC Interrupt routine
 *  MAKE SURE THIS IS RUN IN ARM MODE AND IS IN RAM!!!
 *  GCC will likely require different handling than IAR
 *  to accomplish this.
 *
 * Copied from spi_isr
 */
ARM_CODE RAMFUNC void ssc_isr() {
  
  AT91PS_SSC ssc = AT91C_BASE_SSC;
  

  // This is temporary hack.
  if(ssc_data_tail == NULL)
    return;
  /* Check if both ENDRX and ENDTX flags are set
   * 
   * The transmission isn't done until both are set, as ENDTX will occur
   * at the beginning of the final packet, ENDRX won't occur until after
   * the end of the final packet.
   *
   * CHECK THAT FLAGS ARE CLEARED WHEN WRITING TO PDC COUNTER REGISTERS
   */
  if( ssc->SSC_SR & ( AT91C_SSC_ENDRX | AT91C_SSC_ENDTX ) ) {
    // Disable PDC transfers just to be safe
    ssc->SSC_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;
    // Let the packet's client know we're done
    ssc_data_tail->finished++;
    /* If there is another packet in the list, prep it and start transfer
     * 
     * This is the same code as in ssc_send_packet, but this is run in
     * ARM mode and is in RAM, so it needs to be compilied seperately
     * and can't be put into a function easily AFAIK.
     */
    if( ssc_data_tail->next_packet != NULL ) {
      ssc_data_tail = ssc_data_tail->next_packet;
      ssc->SSC_TPR = (AT91_REG)ssc_data_tail->data_to_write;
      ssc->SSC_RPR = (AT91_REG)ssc_data_tail->read_data;
      // If read_data is NULL, we're ignoring received data
      if( ssc_data_tail->read_data != NULL )
      {
        ssc->SSC_RCR = ssc_data_tail->num_words;
        ssc->SSC_PTCR = AT91C_PDC_RXTEN;
        ssc->SSC_CR = AT91C_SSC_RXEN;
        // We can enable the RXT now as it relies on the TX clock
      }
      else
      {
        // Leave the SSC disabled
        ssc->SSC_RCR = 0;
        ssc->SSC_CR = AT91C_SSC_RXDIS;
      }
      ssc->SSC_TCR = ssc_data_tail->num_words;
      ssc->SSC_PTCR = AT91C_PDC_TXTEN;
    }
    // We're done for now!
    else {
      ssc_data_head = NULL;
      ssc_data_tail = NULL;
    }
  }
}
