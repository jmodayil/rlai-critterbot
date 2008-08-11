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

unsigned int adc_status;
short adc_output[ADC_NUM_CHANNELS];

/**
  * Initialization routine for the adc driver.
  */
int adc_init()
{
	int i;

	// just using software triggers 
	AT91F_ADC_CfgModeReg(AT91C_BASE_ADC, 
											 ADC_RES|ADC_SLEEP|ADC_PRESCAL|ADC_STARTUP|ADC_SHTIM);
	
	// configure the channels
	for (i=0; i < ADC_NUM_CHANNELS; ++i) {
		if( ADC_CHANNEL_MASK & (1 << i) )
			AT91F_ADC_EnableChannel(AT91C_BASE_ADC, 0x1 << i);
	}
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
	AT91F_ADC_StartConversion(AT91C_BASE_ADC);
	return 0;
}

