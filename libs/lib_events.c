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

unsigned int events_status;

unsigned int events_has_event()
{
  unsigned result = events_status;

  // Clear and return the status bit
  events_status = 0;
  return result;
}

/** Initializes the event timer.
  */
void events_init()
{
  AT91PS_AIC pAic = AT91C_BASE_AIC;

  // This will be replaced by the PIT

  // Enable power to the PIO (for sending BLANK/XLAT) and to the TC0
  AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_PIOA );
  AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_TC0 );
  
  // Configure TC0 to run at 100Hz
  AT91C_BASE_TC0->TC_CMR = (AT91C_TC_CLKS_TIMER_DIV5_CLOCK | 
                            AT91C_TC_WAVE | AT91C_TC_WAVESEL_UP_AUTO);
  AT91C_BASE_TC0->TC_RC = TC_TIMER_DIV5_100HZ_RC;

  // Enable the TC0 interrupt to trigger on RC compare
  AT91F_AIC_ConfigureIt(pAic, AT91C_ID_TC0, TC_INTERRUPT_LEVEL,
  AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE, (void*)events_isr);
  AT91F_TC_InterruptEnable(AT91C_BASE_TC0, AT91C_TC_CPCS);
  AT91F_AIC_EnableIt (pAic, AT91C_ID_TC0);

  // Start the timer - do we need the software trigger?
  AT91C_BASE_TC0->TC_CCR =  ( AT91C_TC_CLKEN | AT91C_TC_SWTRG);
  
  events_status = 0;
}

ARM_CODE RAMFUNC void events_isr()
{
  // Test for overrun
  if (events_status)
    error_set (1 << ID_EVENTS);
  // Set the flag
  events_status = 1;
}
