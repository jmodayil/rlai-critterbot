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
#include "lib_except.h"
#include "lib_error.h"
#include "lib_events.h"
#include "armio.h"


extern event_s serial_event_s;
extern event_s ssc_event_s;
extern event_s spi_event_s;
extern event_s leddrive_event_s;
extern event_s ledctl_event_s;
extern event_s accel_event_s;
extern event_s boot_event_s;
extern event_s error_event_s;
extern event_s ui_event_s;
extern event_s motor_event_s;
extern event_s adc_event_s;
extern event_s adcspi_event_s;

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
  int i;
  
  // Enable the PIT interrupt to trigger on an interrupt
  //  Note! We are configuring the system interrupt here; if something else
  //  needs it we will have to deal with this separately.
  AT91F_AIC_ConfigureIt(pAic, AT91C_ID_SYS, PIT_INTERRUPT_LEVEL,
  AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, (void*)events_isr);
  AT91F_AIC_EnableIt (pAic, AT91C_ID_SYS);

  events_status = 0;
  
  events[0] = &serial_event_s;
  events[1] = &ssc_event_s;
  events[2] = &spi_event_s;
  events[3] = &leddrive_event_s;
  events[4] = &ledctl_event_s;
  events[5] = &accel_event_s;
  events[6] = &boot_event_s;
  events[7] = &error_event_s;
  events[8] = &motor_event_s;
  events[9] = &adc_event_s;
  events[10] = &adcspi_event_s;
  events[11] = &ui_event_s;
  
  init_flags = EVENTS_INITS;
  event_flags = EVENTS_DEFAULTS;
 
  // Initialize the console first.
  (*events[0]).init_func();
  
  armprintf("\r\r\r------------CritterBOOT-------------\r\r");
  armprintf("Last reset was a ");
  switch( get_reset_code() ) {
  case 0:
    armprintf("power-up");
    break;
  case 2:
    armprintf("watchdog");
    break;
  case 3:
    armprintf("software");
    break;
  case 4:
    armprintf("external");
    break;
  case 5:
    armprintf("brownout");
    break;
  default:
    armprintf("undefined");
  }
  armprintf(" reset.\r");
  // Initialized any functions with inits.
  for(i = 1; i <= EVENT_MAX; i++) {
    if((init_flags & (1 << i)) && ((*events[i]).init_func != NULL))
    {
      if((*events[i]).init_func()) {
        armprintf("Failed to init EVENT_ID %d\r", i);
        event_flags &= ~(1 << i);
      } else
        armprintf("Initialized EVENT_ID %d\r", i);
    }
  }
  // Enable the PIT interrupt and the PIT itself, as well as setting the
  //  counter max value. We run at a 100Hz. 
  AT91C_BASE_PITC->PITC_PIMR = 
    (EVENTS_PIV_VALUE & AT91C_PITC_PIV) |
    AT91C_PITC_PITIEN | AT91C_PITC_PITEN;
  // Enable user reset 
  *AT91C_RSTC_RMR = 0xA5000101;
}

void event_stop(unsigned int id) {

  if(id > EVENT_MAX)
    return;
  
  event_flags &= ~(1 << id);
}

void event_start(unsigned int id) {
  
  if(id > EVENT_MAX)
    return;
  
  event_flags |= 1 << id;
}

void events_do()
{
  int i, ret_val;

  ping_watchdog();
  for(i = 0; i <= EVENT_MAX; i++) {
    if((event_flags & (1 << i)) && (*events[i]).event_func != NULL) {
      // If an event returns < 0, we will stop it
      // If an event returns > 0, we should do something
      ret_val = (*events[i]).event_func();
      //if(ret_val < 0)
      //  event_flags &= ~(1 << i);
      //if(ret_val > 0)
      //  return;
      (*events[i]).event_count++;
    }
  }
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
    else if (picnt >= 0x00200000)
      error_set (ERR_EVENTSLOW);
    // Set the flag
    events_status = 1;
  }
}
