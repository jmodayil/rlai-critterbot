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
#define EVENT_ID_ADC 9
#define EVENT_ID_ADCSPI 10
#define EVENT_ID_THERMO 11
#define EVENT_ID_UI 12
#define EVENT_ID_RECHARGER 13
#define EVENT_ID_MONITOR 14

#define EVENT_MAX 14


unsigned int init_flags;
unsigned int event_flags;

// Events that should be initialized
#define EVENTS_INITS ( \
    1 << EVENT_ID_UART | \
    1 << EVENT_ID_SSC | \
    1 << EVENT_ID_SPI | \
    1 << EVENT_ID_LEDDRIVE | \
    1 << EVENT_ID_LEDCTL | \
    1 << EVENT_ID_ACCEL | \
    1 << EVENT_ID_MOTOR | \
    1 << EVENT_ID_ADC | \
    1 << EVENT_ID_ADCSPI | \
    1 << EVENT_ID_THERMO | \
    1 << EVENT_ID_UI | \
    1 << EVENT_ID_RECHARGER | \
    1 << EVENT_ID_MONITOR \
    )

// Events started at boot time 
#define EVENTS_DEFAULTS ( \
    1 << EVENT_ID_LEDDRIVE | \
    1 << EVENT_ID_LEDCTL | \
    1 << EVENT_ID_ACCEL | \
    1 << EVENT_ID_MOTOR | \
    1 << EVENT_ID_ADCSPI | \
    1 << EVENT_ID_ADC | \
    1 << EVENT_ID_THERMO | \
    1 << EVENT_ID_UI | \
    1 << EVENT_ID_RECHARGER | \
    1 << EVENT_ID_MONITOR \
    )

/*
 * 'OS' task descripter.
 * init_func    - function called to initialize task
 * event_func   - function called at a frequency of EVENTS_HZ (event handler
 *                for the task)
 * event_count  - number of times a particular event_func has been called
                  since powering up the ARM
 * first_init   - non-zero if the task's initialization has not been performed
 *                since power-up
 */
typedef struct event{
  int (*init_func)();
  int (*event_func)();
  unsigned int event_count;
  int first_init;
}event_s;

extern event_s *events[EVENT_MAX+1];

/** Returns true whether we should call the *_event functions.
  * This will clear the 'has_event' flag.
  * In case of overrun, an error flag will be raised.
  */
unsigned int events_has_event();

/*
 * This global field holds the current event that is being executed.
 * Useful for debugging watchdog resets.
 */
unsigned int current_event;
/*
 * This is where the event that hung is stored if there was a watchdog
 * reset.
 */
unsigned int pre_reset_event;

extern char * events_reset_names[];
extern int num_events_reset_names;
char * events_undefined_reset_name;

void event_stop(unsigned int);
void event_start(unsigned int);
/** Initializes a particular driver. Returns nonzero or the init. function
  * error code (also nonzero) on failure. */
int event_init(unsigned int);

void events_do();

/*
 * Returns the percent of the event loop that the last cycle took.
 * Assumes no overrun.
 */
unsigned char events_time(void);

/** Initializes the event timer.
  */
void events_init();

/** Interrupt service routine for the event timer. */
ARM_CODE RAMFUNC void events_isr();

#endif /* LIB_EVENTS_H */
