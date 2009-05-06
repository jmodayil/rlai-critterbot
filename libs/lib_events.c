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
#include "lib_boot.h"
#include "lib_critical.h"
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
extern event_s mi_recv_event_s;
extern event_s mi_send_event_s;


// Whether a new event cycle is ready to be processed
volatile unsigned int events_status;
// Semapore related to events_status
crit_sem events_status_sem;
// Execution time of the last event cycle
unsigned int event_time;

unsigned int events_has_event()
{
  unsigned int result;

  // Clear and return the status bit
  asm volatile("mov r2,#0\n\t"
               "swp %0, r2, [%1]"
               : "=&r" (result)
               : "r" (&events_status) 
               : "r2" ); 

  // MGB: New code using semaphores (actually not as good as just using SWP,
  //  but doesn't require inline assembly)
/*  crit_get_mutex(&events_status_sem);
  result = events_status;
  events_status = 0;
  crit_release_mutex(&events_status_sem); */
  
  return result;
}

char * events_reset_names[] = {
  "power-up",
  "undefined",
  "watchdog",
  "software",
  "external",
  "brownout" };

int num_events_reset_names = 
  sizeof(events_reset_names) / sizeof(*events_reset_names);

char * events_undefined_reset_name = "undefined";

/** Initializes the event timer.
  */
void events_init()
{
  AT91PS_AIC pAic = AT91C_BASE_AIC;
  int i;
  int reset_code;

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
 
  // Set the first init for everyone
  for (i = 0; i <= EVENT_MAX; i++)
    events[i]->first_init = 1;

  init_flags = EVENTS_INITS;
  event_flags = EVENTS_DEFAULTS;
 
  // Initialize the console first.
  event_init(0);
  
  armprintf("\r\r\r------------CritterBOOT-------------\r\r");
  armprintf("Last reset was a ");

  reset_code = get_reset_code();

  if (reset_code >= 0 && reset_code < num_events_reset_names)
    armprintf(events_reset_names[reset_code]);
  else
    armprintf(events_undefined_reset_name);

  armprintf(" reset.\r");
  // Initialized any functions with inits.
  for(i = 1; i <= EVENT_MAX; i++) {
    if((init_flags & (1 << i)) && (events[i]->init_func != NULL))
    {
      if (event_init(i)) {
        armprintf("Failed to init EVENT_ID %d\r", i);
        // Stop this task, as it failed to init
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
 
  if (events[id]->first_init)
  {
    if (event_init(id))
    {
      armprintf("Failed to init EVENT_ID %d\r", id);
      // In this case, do not actually start the event
      return;
    }
  }
  
  event_flags |= 1 << id;
}

int event_init(unsigned int id) {
  int result;

  if (id > EVENT_MAX)
    return 1;

  if (events[id]->init_func == NULL)
  {
    // Flag the event as having being initialized if there is no init function
    events[id]->first_init = 0;
    return 0;
  }
  
  result = events[id]->init_func();
  // If initialization was successful, label this task as having being
  // initialized
  if (result)
    events[id]->first_init = 1;
  else
    events[id]->first_init = 0;
  
  return result;
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
  event_time = AT91C_BASE_PITC->PITC_PIIR & 0x0000FFFF;
  event_time = (event_time * 100 ) / EVENTS_PIV_VALUE;
}

unsigned char events_time() {
  return event_time;
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
