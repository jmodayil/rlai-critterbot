/*
 *  lib_events.h
 *  
 *
 *  Created by Marc G. Bellemare on 22/07/08
 *
 *  Used to generate a 100Hz event timer.
 *
 */

#ifndef LIB_EVENTS_H
#define LIB_EVENTS_H

#include "compiler.h"
#include "armconfig.h"

#define PIT_INTERRUPT_LEVEL  5
#define EVENTS_HZ 100
// Number of clock cycles (at 3MHz) - 1 before the PIT triggers an interrupt
// We want a 100Hz timer, so we set this to 29999.
#define EVENTS_PIV_VALUE     (MCK / 16 / EVENTS_HZ)

#define EVENT_ID_UART 0
#define EVENT_ID_SSC 1 
#define EVENT_ID_SPI 2
#define EVENT_ID_LEDDRIVE 3
#define EVENT_ID_LEDCTL 4
#define EVENT_ID_ACCEL 5
#define EVENT_ID_BOOT 6
#define EVENT_ID_ERROR 7
#define EVENT_ID_MOTOR 8
#define EVENT_ID_UI 9
#define EVENT_MAX 9


unsigned int init_flags;
unsigned int event_flags;

#define EVENTS_INITS ( \
    1 << EVENT_ID_UART | \
    1 << EVENT_ID_SSC | \
    1 << EVENT_ID_SPI | \
    1 << EVENT_ID_LEDDRIVE | \
    1 << EVENT_ID_MOTOR | \
    1 << EVENT_ID_LEDCTL | \
    1 << EVENT_ID_ACCEL \
    )

#define EVENTS_DEFAULTS ( \
    1 << EVENT_ID_LEDDRIVE | \
    1 << EVENT_ID_LEDCTL | \
    1 << EVENT_ID_MOTOR | \
    1 << EVENT_ID_ACCEL | \
    1 << EVENT_ID_UI \
    )

/*
 * 'OS' task descripter.
 */
typedef struct event{
  int (*init_func)();
  int (*event_func)();
  unsigned int event_count;
}event_s;

event_s *events[EVENT_MAX+1];

/** Returns true whether we should call the *_event functions.
  * This will clear the 'has_event' flag.
  * In case of overrun, an error flag will be raised.
  */
unsigned int events_has_event();

void event_stop(unsigned int);
void event_start(unsigned int);

void events_do();

/** Initializes the event timer.
  */
void events_init();

/** Interrupt service routine for the event timer. */
ARM_CODE RAMFUNC void events_isr();

#endif /* LIB_EVENTS_H */