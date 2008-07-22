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

// @@@ This is all going to be removed when the PIT is used
// Rename!
#define TC_INTERRUPT_LEVEL 5
// Number of clock cycles in 1/100th of a second when using a T/C with
//  clock MCK/1024
#define TC_TIMER_DIV5_100HZ_RC 468

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
