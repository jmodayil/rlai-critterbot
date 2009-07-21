/*
 *  lib_adc.c
 *  
 *
 *  Created by Anna Koop on 23/07/08.
 *
 */

#include "compiler.h"
#include "lib_adc.h"
#include "lib_error.h"
#include "lib_events.h"

event_s adc_event_s = {
  adc_init,
  adc_event,
  0
};

unsigned int adc_status;
unsigned int adc_output[ADC_NUM_CHANNELS];
unsigned int adc_count;

/**
  * Initialization routine for the adc driver.
  */
int adc_init()
{
	volatile int i;

  adc_count = 0;
	// just using software triggers 
	AT91C_BASE_ADC->ADC_MR = (AT91C_ADC_LOWRES_10_BIT | (0x13 << 8) | (0x06 << 16)
     | (0x01 << 24)); 
  //AT91F_ADC_CfgModeReg(AT91C_BASE_ADC, 
  //										 ADC_RES|ADC_SLEEP|ADC_PRESCAL|ADC_STARTUP|ADC_SHTIM);
	
  AT91C_BASE_ADC->ADC_CHER = ADC_CHANNEL_MASK;
	// configure the channels
	//for (i=0; i < ADC_NUM_CHANNELS; ++i) {
//		if( ADC_CHANNEL_MASK & (1 << i) )
//			AT91F_ADC_EnableChannel(AT91C_BASE_ADC, 0x1 << i);
	//}

  AT91F_PIO_CfgOutput(AT91C_BASE_PIOA, 1 << 28);
  AT91F_PIO_SetOutput(AT91C_BASE_PIOA, 1 << 28);
  for(i = 0; i < 100; i++);
  AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, 1 << 28);
  for(i = 0; i < 100; i++);
  AT91F_PIO_SetOutput(AT91C_BASE_PIOA, 1 << 28);
  for(i = 0; i < 100; i++);
  AT91F_ADC_StartConversion(AT91C_BASE_ADC);
  
	return 0;
}

int adc_event()
{
	unsigned int st;
	unsigned int *adc_channels;
	int i;

	adc_channels = (unsigned int *) AT91C_ADC_CDR0;

	st = AT91F_ADC_GetStatus(AT91C_BASE_ADC);
	for (i=0; i < ADC_NUM_CHANNELS; ++i)
	{
		// check EOC flag
		if( st & (1 << i) )
		{
			// read converted data
			adc_output[i] = adc_channels[i];
		}
	}
  if(++adc_count > 100) {
    adc_count = 0;
    AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, 1 << 28);
    for(i = 0; i < 100; i++);
    AT91F_PIO_SetOutput(AT91C_BASE_PIOA, 1 << 28);
    for(i = 0; i < 100; i++);
    AT91F_ADC_StartConversion(AT91C_BASE_ADC);
  }
  return 0;
}

