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
#include "armio.h"

int main()
{
  // serial port should be initialized asap for debugging purposes
  init_serial_port_stdio();
  
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

  while (1)
  {
    if (events_has_event())
    {
      ledctl_event();
      accel_event();

      // Try to keep ui_event last to avoid modifying other stuff
      ui_event();
    }
  }
}
