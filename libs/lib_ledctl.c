/*
 *  lib_ledctl.c
 *  
 *
 *  Created by Marc G. Bellemare on 08/04/08
 *  Last Revision: 30/06/08
 *
 *  The LED driver has two parts. The first one defines code to deal with 
 *   a set of LED controllers. For efficiency, some parts of the code 
 *   directly encode the number of controllers (e.g. ledctl_senddata_all).
 *  
 *  The second part of the driver contains functions which are specific to
 *  the critterbot. In particular, they assume three LED controllers, each
 *  one driving a single color channel for 16 LEDs. It also assumes that
 *  the three LED controllers are chained and that we may send the data
 *  via the SSC on a single line, in color order: blue, green, red.
 *  
 *
 */
#include <stdio.h>
#include <string.h>

#include "compiler.h"
#include "armconfig.h"
#include "lib_ledctl.h"
#include "lib_ssc.h"
#include "lib_AT91SAM7S256.h"
// #include <lib_dbguconsole.c>

// SPI polarity is 0 (from the diagram of the SCLK in led datasheet)
// The LED controller shifts on the rising edge, so SPI uses phase 1

// Rename!
#define TC_INTERRUPT_LEVEL 1
// Number of clock cycles in 1/100th of a second when using a T/C with
//  clock MCK/1024
#define TC_TIMER_DIV5_100HZ_RC 468

// Array holding the 16 grayscale LED values
unsigned short ledctl_txdata[LEDCTL_NUM_CONTROLLERS][LEDCTL_NUM_LEDS];
// Array to receive the 16 status values from the LED controller
unsigned short ledctl_rxdata[LEDCTL_NUM_CONTROLLERS][LEDCTL_NUM_LEDS];
struct ssc_packet ledctl_ssc_packet[LEDCTL_NUM_CONTROLLERS];

unsigned int ledctl_xerr;

unsigned char ledctl_disabled = 1;

// Dot correction data, arranged by channel then LED
// Valid values are between 0 and 63
unsigned char ledctl_dc_data[LEDCTL_NUM_CONTROLLERS][LEDCTL_NUM_LEDS] = {
  {  0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F },
  {  0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F },
  {  0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F }
};

struct ssc_packet ledctl_ssc_packet_dc[LEDCTL_NUM_CONTROLLERS];

void ledctl_newcycle ( void )
{
  if (ledctl_disabled) return;

  // Tell the LED controllers to start a new cycle; the sequence is
  // BLANK-on XLAT-on XLAT-off BLANK-off
  //  This relies a CPU clock cycle being slower than 20ns
  AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_BLANK);
  AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XLAT);
  AT91F_PIO_ClearOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XLAT);
  AT91F_PIO_ClearOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_BLANK);

  // Set the XERR bit if the XERR line is low
  ledctl_xerr = 
    (!AT91F_PIO_IsInputSet ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XERR ));
  // Send the data for the next cycle
  // @@@ Test for finished flag first - do what if not finished?
  ledctl_senddata_all();
}

void ledctl_senddata(int device)
{
  ssc_send_packet(&ledctl_ssc_packet[device]);
}

void ledctl_senddata_all()
{
  int i;

  for(i = 0; i < LEDCTL_NUM_CONTROLLERS; i++)
  {
    // @@@ Error detection: if finished != 0 for some packet, be unhappy
    // This also will require starting with finished = 1
    ledctl_ssc_packet[i].finished = 0;
    ledctl_senddata(i);
  }
}

inline void ledctl_setvalue(int device, int led, int value)
{
  ledctl_txdata[device][led] = value;
}

inline int ledctl_getvalue(int device, int led)
{
  return ledctl_txdata[device][led];
}

inline int ledctl_getstatus(int device, int led)
{
  return (short)ledctl_rxdata[device][led];
}

/** Critterbot specific functions **/

inline void ledctl_setcolor(int led, int red, int green, int blue)
{
  ledctl_txdata[RED_CONTROLLER][led] = red;
  ledctl_txdata[GREEN_CONTROLLER][led] = green;
  ledctl_txdata[BLUE_CONTROLLER][led] = blue;
}

inline int ledctl_getcolor(int led, int color)
{
  return ledctl_txdata[color][led];
}

/** End LED controller core driver **/

RAMFUNC void tc0_irq_handler ( void )
{
  AT91PS_TC pTC = AT91C_BASE_TC0;

  unsigned int status = pTC->TC_SR;
  // Handle RC reached
  if (status & AT91C_TC_CPCS)
  {
    // Begin a new duty cycle for the LED controllers
    ledctl_newcycle();
  }
}

void ledctl_inittimer ( void )
{
  AT91PS_AIC pAic = AT91C_BASE_AIC;

  // Enable power to the PIO (for sending BLANK/XLAT) and to the TC0
  AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_TC0 );
  
  // Configure TC0 to run at 100Hz
  AT91C_BASE_TC0->TC_CMR = (AT91C_TC_CLKS_TIMER_DIV5_CLOCK | 
                            AT91C_TC_WAVE | AT91C_TC_WAVESEL_UP_AUTO);
  AT91C_BASE_TC0->TC_RC = TC_TIMER_DIV5_100HZ_RC;

  // Enable the TC0 interrupt to trigger on RC compare
  AT91F_AIC_ConfigureIt(pAic, AT91C_ID_TC0, TC_INTERRUPT_LEVEL,
  AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE, tc0_irq_handler);
  AT91F_TC_InterruptEnable(AT91C_BASE_TC0, AT91C_TC_CPCS);
  AT91F_AIC_EnableIt (pAic, AT91C_ID_TC0);

  // Start the timer - do we need the software trigger?
  AT91C_BASE_TC0->TC_CCR =  ( AT91C_TC_CLKEN | AT91C_TC_SWTRG);

}

/**
  * Applies dot correction
  */
void ledctl_dc( void ) {
  AT91PS_SSC ssc = AT91C_BASE_SSC;
  int bits_per_word;
  int tfmr_status;
  unsigned int ledctl_is_disabled = ledctl_disabled;
  int i;

  // First, disable the LED controller; we can't disable the 100Hz 
  //  interrupt as others might need it
  ledctl_disable();

  // Store the TFMR status
  tfmr_status = ssc->SSC_TFMR;
  bits_per_word = tfmr_status & AT91C_SSC_DATLEN;

  // Set up the SSC to send 6 bits words, leaving the rest of the mode
  //  register unchanged
  ssc->SSC_TFMR = (5 & AT91C_SSC_DATLEN) | (tfmr_status & ~AT91C_SSC_DATLEN);
  // Disable receiving (@@@ should we receive something?)
  ssc->SSC_CR = AT91C_SSC_RXDIS;
  
  // Change to dot correction mode
  AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_MODE );
  ledctl_ssc_packet_dc[LEDCTL_NUM_CONTROLLERS-1].finished = 0;

  // Send all three DC packets
  for(i = 0; i < LEDCTL_NUM_CONTROLLERS; i++)
    ssc_send_packet(&ledctl_ssc_packet_dc[i]);

  // Wait for the data to have been sent
  while(!(ledctl_ssc_packet_dc[LEDCTL_NUM_CONTROLLERS-1].finished));

  // We need to XLAT to send the DC data in
  AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XLAT);
  AT91F_PIO_ClearOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XLAT);

  // Reset the mode to grayscale mode 
  AT91F_PIO_ClearOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_MODE);
  
  // Reset the previous configuration
  ssc->SSC_TFMR = bits_per_word | tfmr_status;
  // Enable receiving again
  ssc->SSC_CR = AT91C_SSC_RXEN;
  // Enable the LED driver
  if (!ledctl_is_disabled)
    ledctl_enable();

}

void ledctl_disable()
{
  ledctl_disabled = 1;
}

void ledctl_enable()
{
  ledctl_disabled = 0;
}

unsigned int ledctl_data_sent()
{
  return (ledctl_ssc_packet[LEDCTL_NUM_CONTROLLERS-1].finished > 0);
}

void ledctl_init( void )
{
  int i, l;

  armprintf ("LEDCTL init packets\n");

  // Initalize the packets for both data and dot correction
  for (i = 0; i < LEDCTL_NUM_CONTROLLERS; i++)
  {
    ledctl_ssc_packet[i].num_words = LEDCTL_NUM_LEDS;
    ledctl_ssc_packet[i].data_to_write = ledctl_txdata[i];
    ledctl_ssc_packet[i].read_data = ledctl_rxdata[i];
    ledctl_ssc_packet_dc[i].num_words = LEDCTL_NUM_LEDS;
    ledctl_ssc_packet_dc[i].data_to_write = (unsigned short*)ledctl_dc_data[i];
    ledctl_ssc_packet_dc[i].read_data = NULL;

    // Initialize LED values to 0
    for (l = 0; l < LEDCTL_NUM_LEDS; l++)
    {
      // Set both transmit and receive buffers to 0 values
      // @@@ replace this by a memset ?
      ledctl_txdata[i][l] = 0;
      ledctl_rxdata[i][l] = 0;
    }
  }
  
  // Configure the PIO pins properly
  AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_PIOA );
  
  AT91F_PIO_CfgOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_BLANK );
  AT91F_PIO_CfgOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XLAT );
  AT91F_PIO_CfgInput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XERR );
  AT91F_PIO_CfgOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_MODE );
 
  armprintf ("LEDCTL dot correction\n");

  // Run dot-correction initially
  ledctl_dc(); 
  
  armprintf ("LEDCTL send data\n");
  // Send the initial LED data, which should be all 0's
  ledctl_senddata_all();
  // Wait for the data to have been sent
  while (!ledctl_data_sent()) ;

  // Now BLANK and XLAT in order to load the new data into the LED controllers
  AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_BLANK);
  AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XLAT);
  AT91F_PIO_ClearOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XLAT);
  AT91F_PIO_ClearOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_BLANK);
 
  // Start the grayscale clock - must be done after dot correction and
  //  sending the LED data to avoid 'flashing' the LEDs

  // Enable power to the PWM controller for GSClock
  AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_PWMC );
  // Setup the PWM
  AT91C_BASE_PIOA->PIO_PDR = AT91C_PA23_PWM0;
  AT91C_BASE_PIOA->PIO_BSR = AT91C_PA23_PWM0;

  AT91C_BASE_PWMC->PWMC_MR = 1 | AT91C_PWMC_PREA_MCK;
  AT91C_BASE_PWMC->PWMC_ENA = AT91C_PWMC_CHID0;
  AT91C_BASE_PWMC->PWMC_CH[0].PWMC_CMR = AT91C_PWMC_CPRE_MCKA;
  AT91F_PWMC_CfgChannel(AT91C_BASE_PWMC, 0, AT91C_PWMC_CPRE_MCKA, 117, 58);
  AT91F_PWMC_StartChannel(AT91C_BASE_PWMC, 0);

  // Enable the LED driver
  ledctl_enable();
}

