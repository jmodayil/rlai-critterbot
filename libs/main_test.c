/*
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
#include "lib_boot.h"
#include "lib_spi.h"
#include "lib_ssc.h"
#include "lib_ledctl.h"
#include "lib_accel.h"
#include "lib_ui.h"
#include "lib_events.h"
#include "lib_error.h"
#include "lib_leddrive.h"
#include "armio.h"

ARM_CODE RAMFUNC void C_DAbt_Handler(unsigned int cal_loc) {
  
  __armprintf("Calling Address: %p\r", cal_loc);
  __armprintf("ASR: %x\r", *AT91C_MC_ASR);
  __armprintf("AASR: %p\r", *AT91C_MC_AASR);
  while(1);
}

ARM_CODE RAMFUNC void spur_isr() {
  __armputchar('?');
  error_set(ERR_SPURINT);
}

int main()
{
  // serial port should be initialized asap for debugging purposes
  //init_serial_port_stdio();
  
  AT91C_BASE_AIC->AIC_SPU = (unsigned int)spur_isr;
  // SPI has no dependencies
  //spi_init();
  // SSC has no dependencies
  //ssc_init();
  
  // Ledctl must be initialized after SSC
  //ledctl_init();
  // Accel must be initialized after SPI
  //accel_init();

  //leddrive_init();
  // Events has no dependencies
  events_init();

  armprintf("critterprompt> ");
  while (1)
  {
    if (events_has_event())
    {
      events_do();

      //error_disp();
      //leddrive_event();
      //ledctl_event();
      //accel_event();
      // Try to keep ui_event last to avoid modifying other stuff
      //ui_event();
      //boot_event();
      //armprintf(":%d\n", seq);
    }
  }
}
