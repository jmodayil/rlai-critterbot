/*
 *  lib_ledctl.c
 *  
 *
 *  Created by Marc G. Bellemare on 08/04/08
 *  Last Revision: 30/06/08
 *
 */
#include <stdio.h>
#include <string.h>

#include "lib_ledctl.h"
#include "lib_ssc.h"
// #include <lib_dbguconsole.c>

// SPI polarity is 0 (from the diagram of the SCLK in led datasheet)
// The LED controller shifts on the rising edge, so SPI uses phase 1
// For now, we are using slave 0 in the SPI

// Number of LEDs per controller
#define LEDCTL_NUM_LEDS 16

// Number of controllers
#define LEDCTL_NUM_CONTROLLERS 3

// I/O lines connected to the BLANK/XLAT inputs of the LED controllers
#define LEDCTL_PIN_BLANK 4
#define LEDCTL_PIN_XLAT 5

#define TC_INTERRUPT_LEVEL 6
// Number of clock cycles in 1/100th of a second when using a T/C with
//  clock MCK/1024
#define TC_TIMER_DIV5_100HZ_RC 468

// Array holding the 16 grayscale LED values
unsigned int ledctl_txdata[LEDCTL_NUM_CONTROLLERS][LEDCTL_NUM_LEDS];
// Array to receive the 16 status values from the LED controller
unsigned int ledctl_rxdata[LEDCTL_NUM_CONTROLLERS][LEDCTL_NUM_LEDS];
struct spi_packet ledctl_spi_packet[LEDCTL_NUM_CONTROLLERS];

void ledctl_newcycle ( void )
{
  // Tell the LED controllers to start a new cycle; the sequence is
  // BLANK-on XLAT-on XLAT-off BLANK-off
  //  This relies a CPU clock cycle being slower than 20ns
  AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, LEDCTL_PIN_BLANK);
  AT91F_PIO_SetOutput ( AT91C_BASE_PIOA, LEDCTL_PIN_XLAT);
  AT91F_PIO_ClearOutput ( AT91C_BASE_PIOA, LEDCTL_PIN_XLAT);
  AT91F_PIO_ClearOutput ( AT91C_BASE_PIOA, LEDCTL_PIN_BLANK);
  
  // Send the data for the next cycle
  // @@@ Test for finished flag first - do what if not finished?
  ledctl_senddata_all();
}

void ledctl_senddata(int device)
{
  spi_send_packet(&ledctl_spi_packet[device]);
}

void ledctl_senddata_all()
{
  // Chunk out the four SPI packets into the SPI driver
  spi_send_packet(&ledctl_spi_packet[0]);
  spi_send_packet(&ledctl_spi_packet[1]);
  spi_send_packet(&ledctl_spi_packet[2]);
  spi_send_packet(&ledctl_spi_packet[3]);
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
  return ledctl_rxdata[device][led];
}

inline int ledctl_setled(int led, int red, int green, int blue)
{
  
}

/** End LED controller core driver **/

void tc0_irq_handler ( void )
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

void ledctl_init( void )
{
  int i, l;
  
  // Initalize the SPI packet
  for (i = 0; i < LEDCTL_NUM_CONTROLLERS; i++)
  {
    ledctl_spi_packet[i].num_words = LEDCTL_NUM_LEDS;
    // Assume that the LED controllers are on CS0-3
    ledctl_spi_packet[i].device_id = i;
    ledctl_spi_packet[i].data_to_write = ledctl_txdata[i];
    ledctl_spi_packet[i].read_data = ledctl_rxdata[i];
  
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
  
  AT91F_PIO_CfgOutput ( AT91C_BASE_PIOA, LEDCTL_PIN_BLANK );
  AT91F_PIO_CfgOutput ( AT91C_BASE_PIOA, LEDCTL_PIN_XLAT );

}

int main()
{
  char statusString[256];
  char commandString[256];

  char * pStr;
  int len;
  int num_chars;
  int i;
  int hasCommand = 0;
  unsigned int ledNum;
  unsigned int base;

  // Initialize the SPI - assume no one else is doing so
  spi_init();
  // Now initialize a timer/counter at 100Hz
  ledctl_inittimer();
  
  ledctl_init();

  while (1)
  {
    hasCommand = 0;

    // Read in any command
    if ( dbgu_read_string(commandString, 256) == 0 )
    {
      hasCommand = 1;
      // Parse it, value by value (use strsep instead?)
      pStr = strtok(commandString, " ");
      ledNum = atoi(pStr);
      base = ledNum * 3;

      // Read in the three channels
      for (i = 0; pStr && i < 3; pStr = strtok(NULL, " "), i++)
      {
        // Only set LEDs on device 0 for now
        ledctl_setvalue(0, i + base, atoi(pStr));
      }
      
      // Error on malformed data, unless it's a newline
      if (i < 3 || ledNum > 4)
        if (commandString[0] != 0)
          dbgu_write_string ("Bad command or file name.");
    }
    
    // Print out status if we received a command
    if (hasCommand)
    {
      pStr = statusString;
      len = 256;

      // Move pStr forward by the number of characters printed
      num_chars = snprintf (statusString, 256, "Status:");
      pStr += num_chars;
      len -= num_chars;

      for (i = 0; i < LEDCTL_NUM_LEDS; i++)
      {
        num_chars = snprintf (pStr, len, " %x", ledctl_getstatus(0, i)); 
      }

      // print to serial
      dbgu_write_string (statusString);
    }
  }
}
