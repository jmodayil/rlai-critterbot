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
#include "armio.h"

event_s ledctl_event_s = {
  ledctl_init,
  ledctl_event,
  0
};

// SPI polarity is 0 (from the diagram of the SCLK in led datasheet)
// The LED controller shifts on the rising edge, so SPI uses phase 1

// Rename!
#define TC_INTERRUPT_LEVEL 1
// Number of clock cycles in 1/100th of a second when using a T/C with
//  clock MCK/1024
#define TC_TIMER_DIV5_100HZ_RC 468

// Array holding the 16 grayscale LED values
// A note about this buffer - ledctl_txdata[x][0] actually refers to the
//  colors of LED 15, because the data is sent backwards. Use ledctl_setcolor.
unsigned short ledctl_txdata[LEDCTL_NUM_CONTROLLERS][LEDCTL_NUM_LEDS];
// Array to receive the 16 status values from the LED controller
unsigned short ledctl_rxdata[LEDCTL_NUM_CONTROLLERS][LEDCTL_NUM_LEDS];
struct ssc_packet ledctl_ssc_packet[LEDCTL_NUM_CONTROLLERS];

unsigned int ledctl_xerr;

enum ledctl_state_type ledctl_state = GRAYSCALE;

// Dot correction data, arranged by channel then LED
// Valid values are between 0 and 63
//  Remember that ledctl_dc_data[x][0] corresponds to the 15th LED and 
//  vice-versa (set_dc takes care of this)
unsigned char ledctl_dc_data[LEDCTL_NUM_CONTROLLERS][LEDCTL_NUM_LEDS] = {
  {  0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F },
  {  0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F },
  {  0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F }
};
//unsigned char ledctl_dc_rxdata[LEDCTL_NUM_CONTROLLERS][LEDCTL_NUM_LEDS];

struct ssc_packet ledctl_ssc_packet_dc[LEDCTL_NUM_CONTROLLERS];

int ledctl_event ( void )
{
  AT91PS_SSC ssc = AT91C_BASE_SSC;
  int tfmr_status;

  // Tell the LED controllers to start a new cycle; the sequence is
  // BLANK-on XLAT-on XLAT-off BLANK-off (if not first cycle of mode)
  //  This relies a CPU clock cycle being slower than 20ns
  // Also, we should never reach this part of the code in GRAYSCALE or
  //  DOT_CORRECTION state without having sent the data previously; hence
  //  the XLAT is valid. If the state is FIRST_*, then BLANK will not be
  //  de-asserted, and so the XLAT is harmless.
  AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_BLANK);
  AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XLAT);
  AT91F_PIO_ClearOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XLAT);

  // Set the XERR bit if the XERR line is low
  // @@@ move me out
  ledctl_xerr = 
    (!AT91F_PIO_IsInputSet ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XERR ));
  
  // Send the data for the next cycle
  // @@@ Test for finished flag first - do what if not finished?
  switch (ledctl_state)
  {
    case GRAYSCALE:
      AT91F_PIO_ClearOutput (AT91C_BASE_PIOA, 1 << LEDCTL_PIN_BLANK);
      ledctl_senddata_all();
      break;
    case FIRST_GRAYSCALE:
      // Set up the SSC to send 12 bits words, leaving the rest of the mode
      //  register unchanged
      tfmr_status = ssc->SSC_TFMR;
      ssc->SSC_TFMR = (LEDCTL_GS_WORD_SIZE & AT91C_SSC_DATLEN) | 
        (tfmr_status & ~AT91C_SSC_DATLEN);
      
      AT91F_PIO_ClearOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_MODE ); 
      ledctl_senddata_all();
      ledctl_state = GRAYSCALE;
      break;
    case DOT_CORRECTION:
      ledctl_senddata_dc();
      AT91F_PIO_ClearOutput (AT91C_BASE_PIOA, 1 << LEDCTL_PIN_BLANK);
      break;
    case FIRST_DOT_CORRECTION:
      // Set up the SSC to send 6 bits words
      tfmr_status = ssc->SSC_TFMR;
      ssc->SSC_TFMR = (LEDCTL_DC_WORD_SIZE & AT91C_SSC_DATLEN) | 
        (tfmr_status & ~AT91C_SSC_DATLEN);
      
      AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_MODE ); 
      ledctl_senddata_dc();
      ledctl_state = DOT_CORRECTION;
      break;
  }
  return 0;
}

void ledctl_senddata_all()
{
  int i;

  for(i = 0; i < LEDCTL_NUM_CONTROLLERS; i++)
  {
    if(ledctl_ssc_packet[i].finished == 0) {
      error_set(ERR_LEDCTL);
      error_set(ERR_SSC_OVERFLOW);
    }
    
    ledctl_ssc_packet[i].finished = 0;
    ssc_send_packet(&ledctl_ssc_packet[i]);
  }
}

void ledctl_senddata_dc()
{
  int i;

  for(i = 0; i < LEDCTL_NUM_CONTROLLERS; i++)
  {
    if(ledctl_ssc_packet_dc[i].finished == 0) {
      error_set(ERR_LEDCTL);
      error_set(ERR_SSC_OVERFLOW);
    }
    
    ledctl_ssc_packet_dc[i].finished = 0;
    ssc_send_packet(&ledctl_ssc_packet_dc[i]);
  }
}

inline void ledctl_setvalue(int device, int led, int value)
{
  ledctl_txdata[device][LEDCTL_NUM_LEDS-1-led] = value;
}

inline int ledctl_getvalue(int device, int led)
{
  return ledctl_txdata[device][LEDCTL_NUM_LEDS-1-led];
}

inline int ledctl_getstatus(int device, int led)
{
  return (short)ledctl_rxdata[device][led];
}

/** Critterbot specific functions **/

inline void ledctl_setcolor(int led, int red, int green, int blue)
{
  // The compiler should be smart enough to hardcode this?
  unsigned int base=LEDCTL_NUM_LEDS-1;

  ledctl_txdata[RED_CONTROLLER][base-led] = red;
  ledctl_txdata[GREEN_CONTROLLER][base-led] = green;
  ledctl_txdata[BLUE_CONTROLLER][base-led] = blue;
}

inline int ledctl_getcolor(int led, int color)
{
  return ledctl_txdata[color][LEDCTL_NUM_LEDS-1-led];
}

inline void ledctl_setdc(int led, int red, int green, int blue)
{
  unsigned int base=LEDCTL_NUM_LEDS-1;

  ledctl_dc_data[RED_CONTROLLER][base-led] = red;
  ledctl_dc_data[GREEN_CONTROLLER][base-led] = green;
  ledctl_dc_data[BLUE_CONTROLLER][base-led] = blue;
}

inline int ledctl_getdc(int led, int color)
{
  return ledctl_dc_data[color][LEDCTL_NUM_LEDS-1-led];
}

unsigned short ledctl_getLOD (int color)
{
  // The LOD data lies in two 12-bits chunks, the first to be clocked in
  //  for each controller.
  unsigned int twelvebits = ledctl_rxdata[color][0] & 0xFFF;
  unsigned int fourbits = ledctl_rxdata[color][1] >> 8;

  // The MSB of the LOD data corresponds to LED 15; we keep it this way
  return ((twelvebits << 4) | fourbits); 
}

unsigned int ledctl_getTEF()
{
  // The TEF comes after the LOD data and is the 17th bit of data. As such,
  //  it is the 9th MSB of the second chunk of data (recall that we read
  //  12 bits words from the SSC).
  unsigned int result = 0;
  int i;

  // Store each TEF in a corresponding bit
  for (i = 0; i < LEDCTL_NUM_CONTROLLERS; i++)
    result |= ((ledctl_rxdata[i][1] & 0x0800) >> 7) << i;

  return result;
}

unsigned int ledctl_geterr()
{
  return ledctl_xerr;
}

void ledctl_setmode(enum ledctl_state_type mode)
{
  // To avoid interfering with the 100Hz timer, this simply flags the
  //  state as FIRST_*. The proper changes are applied during the next 
  //  100Hz cycle.
  switch (mode)
  {
    case GRAYSCALE:
    case FIRST_GRAYSCALE:
      ledctl_state = FIRST_GRAYSCALE;
      break;
    case DOT_CORRECTION:
    case FIRST_DOT_CORRECTION:
      ledctl_state = FIRST_DOT_CORRECTION;
      break;
    default:
      // Raise some sort of error flag?
      armprintf ("Invalid mode in ledctl_setmode\r");
      break;
  }
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
    ledctl_event();
  }
  /*startup(2);
  for(i=0;i<16;i++) {
    ledctl_setcolor(i, LED[i].r, LED[i].g, LED[i].b);
  }*/
}

void ledctl_inittimer ( void )
{
  AT91PS_AIC pAic = AT91C_BASE_AIC;

  // Enable power to the PIO (for sending BLANK/XLAT) and to the TC0
  AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_PIOA );
  AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_TC0 );
  
  // Configure TC0 to run at 100Hz
  AT91C_BASE_TC0->TC_CMR = (AT91C_TC_CLKS_TIMER_DIV5_CLOCK | 
                            AT91C_TC_WAVE | AT91C_TC_WAVESEL_UP_AUTO);
  AT91C_BASE_TC0->TC_RC = TC_TIMER_DIV5_100HZ_RC;

  // Enable the TC0 interrupt to trigger on RC compare
  AT91F_AIC_ConfigureIt(pAic, AT91C_ID_TC0, TC_INTERRUPT_LEVEL,
  AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE, (void*)tc0_irq_handler);
  AT91F_TC_InterruptEnable(AT91C_BASE_TC0, AT91C_TC_CPCS);
  AT91F_AIC_EnableIt (pAic, AT91C_ID_TC0);

  // Start the timer - do we need the software trigger?
  AT91C_BASE_TC0->TC_CCR =  ( AT91C_TC_CLKEN | AT91C_TC_SWTRG);

}

/**
  * Applies initial dot correction. The grayscale clock should be disabled
  *  when this is called, to avoid flashing LEDs.
  */
void ledctl_dc( void ) {
  AT91PS_SSC ssc = AT91C_BASE_SSC;
  int tfmr_status;
  
  // Blank as a precaution
  AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_BLANK);

  // We have to set the MODE manually here, as well as word size in the SSC
  tfmr_status = ssc->SSC_TFMR;
  ssc->SSC_TFMR = (LEDCTL_DC_WORD_SIZE & AT91C_SSC_DATLEN) | 
      (tfmr_status & ~AT91C_SSC_DATLEN);
      
  AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_MODE ); 

  // Send the DC data
  ledctl_senddata_dc();
  // Wait for the data to have been sent
  while(!(ledctl_ssc_packet_dc[LEDCTL_NUM_CONTROLLERS-1].finished));
  // We need to XLAT to get the DC data in
  AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XLAT);
  AT91F_PIO_ClearOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XLAT);

  ledctl_setmode(FIRST_GRAYSCALE);

  // Leave BLANK asserted until the next cycle to avoid flashing lights
}

unsigned int ledctl_data_sent()
{
  return (ledctl_ssc_packet[LEDCTL_NUM_CONTROLLERS-1].finished > 0);
}


void ledctl_init_packets()
{
  int i, l;

  // Initalize the packets for both data and dot correction
  for (i = 0; i < LEDCTL_NUM_CONTROLLERS; i++)
  {
    ledctl_ssc_packet[i].num_words = LEDCTL_NUM_LEDS;
    ledctl_ssc_packet[i].data_to_write = ledctl_txdata[i];
    // We reverse the buffers for reading, as the data comes back backwards
    ledctl_ssc_packet[i].read_data = ledctl_rxdata[i];//LEDCTL_NUM_CONTROLLERS-1-i];
    ledctl_ssc_packet[i].finished = 1;
    ledctl_ssc_packet_dc[i].num_words = LEDCTL_NUM_LEDS;
    ledctl_ssc_packet_dc[i].data_to_write = (unsigned short*)ledctl_dc_data[i];
    ledctl_ssc_packet_dc[i].read_data = NULL;
    ledctl_ssc_packet[i].finished = 1;
    
    // Initialize LED values to 0
    for (l = 0; l < LEDCTL_NUM_LEDS; l++)
    {
      // Set both transmit and receive buffers to 0 values
      ledctl_txdata[i][l] = 0;
      ledctl_rxdata[i][l] = 0;
    }
  }
}

/** Initializes, but does not start, the grayscale clock sent using PWM */
void ledctl_init_gsclock()
{
  AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_PWMC );
  // Setup the PWM
  AT91C_BASE_PIOA->PIO_PDR = AT91C_PA23_PWM0;
  AT91C_BASE_PIOA->PIO_BSR = AT91C_PA23_PWM0;

  AT91C_BASE_PWMC->PWMC_MR = 1 | AT91C_PWMC_PREA_MCK;
  AT91C_BASE_PWMC->PWMC_ENA = AT91C_PWMC_CHID0;
  AT91C_BASE_PWMC->PWMC_CH[0].PWMC_CMR = AT91C_PWMC_CPRE_MCKA;
  AT91F_PWMC_CfgChannel(AT91C_BASE_PWMC, 0, AT91C_PWMC_CPRE_MCKA, 117, 58);
  AT91F_PWMC_StartChannel(AT91C_BASE_PWMC, 0);
}

int ledctl_init( void )
{

  ledctl_init_packets();
  
  // Configure the PIO pins properly
  AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_PIOA );
  
  AT91F_PIO_CfgOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_BLANK );
  AT91F_PIO_CfgOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XLAT );
  AT91F_PIO_CfgInput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XERR );
  AT91F_PIO_CfgPullup( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_XERR );
  AT91F_PIO_CfgOutput ( AT91C_BASE_PIOA, 1 << LEDCTL_PIN_MODE );
 
  // Run dot-correction initially; it will set the state to FIRST_GRAYSCALE,
  //  allowing us to begin clock grayscale data. BLANK is also left high 
  //  and will be de-asserted only in two 100Hz cycles.
  ledctl_dc(); 
  
  // Initialize the grayscale clock
  ledctl_init_gsclock();
  return 0;
}

