/*
 *  lib_adc.h
 *  
 *
 *  Created by Anna Koop on 23/07/08.
 *
 *  Minimum startup time before data becomes valid: ?.
 */

#include "AT91SAM7S256.h"
#include "compiler.h"
//#include "lib_events.h" ??
#include "lib_AT91SAM7S256.h"
#include "armio.h"

#ifndef LIB_ADC_H
#define LIB_ADC_H

// do I need to keep track of the delay?
#define ADC_NUM_CHANNELS 8
#define ADC_CHANNEL_MASK 0xF0

// SLEEP - save power if necessary
#define ADC_SLEEP 0x0 << 5
// 0 for 8 bits, 1 for 10
#define ADC_RES 0x0 << 4

#define ADCCLOCK_FREQ 3000000
#define ADC_STARTUP_TIME .00002
#define ADC_SHTIM_S .000005

#define ADC_PRESCAL ((unsigned int) (MCK/(2*ADCCLOCK_FREQ)-1) << 8)
#define ADC_STARTUP ((unsigned int) (ADC_STARTUP_TIME*ADCCLOCK_FREQ/8) << 16)
#define ADC_SHTIM ((unsigned int) (ADC_SHTIM_S*ADCCLOCK_FREQ-1) << 24)

extern unsigned int adc_status;
extern short adc_output[ADC_NUM_CHANNELS]; 


/**
  * Initialization routine for the adc driver.
  */
int adc_init();

/**
  * Event function called by the 100Hz timer.
  */
int adc_event();


/** Returns the adc-provided status. */
__inline unsigned char adc_get_status()
{
  return adc_status;
}

/** Returns the current output of the adc for a particular channel.
  */
__inline short adc_get_output(int channel)
{
	return adc_output[channel];
}

#endif /* LIB_ADC_H */

