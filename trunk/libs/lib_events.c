/*
 *  lib_events.c
 *  
 *
 *  Created by Marc G. Bellemare on 22/07/08
 *
 *  Used to generate a 100Hz event timer.
 *
 */

#include "compiler.h"
#include "armconfig.h"

#include "AT91SAM7S256.h"
#include "lib_AT91SAM7S256.h"

#include "lib_error.h"
#include "lib_events.h"
#include "armio.h"

extern unsigned int seq;

volatile unsigned int events_status;

unsigned int events_has_event()
{
  unsigned int result;

  //AT91C_BASE_PITC->PITC_PIMR &= ~AT91C_PITC_PITIEN;  
  //AT91F_AIC_DisableIt(AT91C_BASE_AIC, AT91C_ID_SYS);
  // 
  // result = events_status;
  // Clear and return the status bit
  // events_status = 0;
  asm volatile("mov r2,#0\n\t"
               "swp %0, r2, [%1]"
               : "=&r" (result)
               : "r" (&events_status) 
               : "r2" );
  //AT91F_AIC_EnableIt(AT91C_BASE_AIC, AT91C_ID_SYS);

  return result;
}

/** Initializes the event timer.
  */
void events_init()
{
  AT91PS_AIC pAic = AT91C_BASE_AIC;

  // Enable the PIT interrupt to trigger on an interrupt
  //  Note! We are configuring the system interrupt here; if something else
  //  needs it we will have to deal with this separately.
  AT91F_AIC_ConfigureIt(pAic, AT91C_ID_SYS, PIT_INTERRUPT_LEVEL,
  AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, (void*)events_isr);
  AT91F_AIC_EnableIt (pAic, AT91C_ID_SYS);

  events_status = 0;
  
  // Enable the PIT interrupt and the PIT itself, as well as setting the
  //  counter max value. We run at a 100Hz. 
  AT91C_BASE_PITC->PITC_PIMR = 
    (EVENTS_PIV_VALUE & AT91C_PITC_PIV) |
    AT91C_PITC_PITIEN | AT91C_PITC_PITEN;
      
  armprintf("Initialized event timer.\n");
}

ARM_CODE RAMFUNC void events_isr()
{
  AT91PS_PITC pitc = AT91C_BASE_PITC;

  unsigned int status = pitc->PITC_PISR;
  unsigned int picnt;

  if(status & AT91C_PITC_PITS) {
    // Clear the interrupt status by reading the PIVR register
    picnt = pitc->PITC_PIVR;

    /* Test for overrun: either an event was not processed or for some
     *  obscure reason, the PIT counted twice (in this case, PICNT > 1).
     * The 12 MSB of the PITC_PIVR register are the ones of interest; only
     *  the lowest one should be set.
     */
    if (events_status != 0)
      error_set (ERR_EVENTS);
    if (picnt >= 0x00200000)
      error_set (ERR_EVENTSLOW);
    // Set the flag
    events_status = 1;
    seq++;
  }
}
