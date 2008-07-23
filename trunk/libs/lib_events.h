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

#define PIT_INTERRUPT_LEVEL  5
// Number of clock cycles (at 3MHz) - 1 before the PIT triggers an interrupt
// We want a 100Hz timer, so we set this to 29999.
#define EVENTS_PIV_VALUE     29999

/** Returns true whether we should call the *_event functions.
  * This will clear the 'has_event' flag.
  * In case of overrun, an error flag will be raised.
  */
unsigned int events_has_event();

/** Initializes the event timer.
  */
void events_init();

/** Interrupt service routine for the event timer. */
ARM_CODE RAMFUNC void events_isr();

#endif /* LIB_EVENTS_H */
