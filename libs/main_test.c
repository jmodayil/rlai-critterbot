*
 *  main.c
 *  
 *
 *  Created by Marc G. Bellemare on 20/07/08
 *
 *  This main.c file implements the main function for the middle board 
 *  AT9SAM7S256 chip.
 */

#include <math.h>

#include "compiler.h"
#include "armconfig.h"

#include "lib_spi.h"
#include "lib_ssc.h"
#include "lib_ledctl.h"
#include "lib_accel.h"
#include "lib_ui.h"
#include "lib_events.h"
#include "lib_error.h"
#include "lib_leddrive.h"
#include "armio.h"

unsigned int seq;

ARM_CODE RAMFUNC spur_isr() {
  error_set(ERR_SPURINT);
}

int run_ui = 1;

int main()
{
 
  int g_x, g_y;
  int g_dir, g_mag;
  float ang;
  
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

  leddrive_init();
  // Events has no dependencies
  events_init();

  armprintf("Entering while loop...\n");
  seq = 0;
  while (1)
  {
    if (events_has_event())
    {
      // events_do();
      // Temporary LED / accelerometer test.

      g_x = accel_get_output(0);
      g_y = accel_get_output(1);

      g_mag = 4 * (unsigned int)sqrtf(g_x*g_x + g_y*g_y);
      if(g_mag > 4095)
        g_mag = 4095; 
      
      g_dir = (int)((atan2(-g_y, g_x) / 3.1415927) * (float)180);      
      armprintf("%d \n", g_dir);
      if(leddrive_state != STARTUP) {
        leddrive_angle(&g_dir, &g_mag);
      }

      // END LED test.
      error_disp();
      leddrive_event();
      ledctl_event();
      accel_event();
      // Try to keep ui_event last to avoid modifying other stuff
      if (run_ui)
        ui_event();
      boot_event();
      //armprintf(":%d\n", seq);
    }
  }
}
