/*
 *  main.c
 *  
 *
 *  Created by Marc G. Bellemare on 20/07/08
 *
 *  This main.c file implements the main function for the middle board 
 *  AT9SAM7S256 chip.
 */

#include "compiler.h"
#include "armconfig.h"

#include "lib_spi.h"
#include "lib_ssc.h"
#include "lib_ledctl.h"
#include "lib_accel.h"
#include "lib_ui.h"
#include "lib_events.h"
#include "lib_error.h"
#include "armio.h"

unsigned int seq;

ARM_CODE RAMFUNC spur_isr() {
  AT91C_BASE_US0->US_THR = 0x3F;
  while(1);
}

int main()
{
 
  unsigned int count1, count2; 
  // serial port should be initialized asap for debugging purposes
  init_serial_port_stdio();
  
  AT91C_BASE_AIC->AIC_SPU = (void*) spur_isr;
  // SPI has no dependencies
  spi_init();
  // SSC has no dependencies
  ssc_init();
  
  // Ledctl must be initialized after SSC
  ledctl_init();
  // Accel must be initialized after SPI
  accel_init();

  // Events has no dependencies
  events_init();

  armprintf("Entering while loop...\n");
  seq = 0;
  while (1)
  {
    if (events_has_event())
    {
      error_disp();
      ledctl_event();
      accel_event();
      // Try to keep ui_event last to avoid modifying other stuff
      ui_event();
      //armprintf(":%d\n", seq);
    }
  }
}
