/*
 *  lib_ui_test.c
 *  
 *
 *  Created by Marc G. Bellemare on 7/07/08.
 *  Last Revision: 7/07/08
 *
 *
 *  Code to test lib_ui.c, the user interface.
 *
 */

#include <stdio.h>
#include "compiler.h"
#include "armconfig.h"

#include "armio.h"
#include "lib_ledctl.h"
#include "lib_ssc.h"
#include "lib_ui.h"

int main()
{
  // Initialize the serial port and the LED controller
  init_serial_port_stdio();
  armprintf("Hello, World.\n");
  armprintf("sizeof(unsigned short) = %d.\n", sizeof(unsigned short));
  ssc_init();
  armprintf("Initialized ssc.\n"); 
  ledctl_init();
  armprintf("Initialized ledctl.\n");
  // @@@ This function will have to be replaced when we get a proper 100Hz
  //  event clock
  ledctl_inittimer();
  armprintf("Initialized timer.\n");
  // Repeatedly loop for events
  while (1)
  {
    ui_event();
  }
}
