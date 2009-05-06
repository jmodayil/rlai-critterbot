/*
 *  lib_error.h
 *  
 *
 *  Created by Marc G. Bellemare on 21/07/08.
 *
 *  Error-reporting driver.
 */


#ifndef LIB_ERROR_H
#define LIB_ERROR_H

#include "lib_ledctl.h"
#include "lib_events.h"
#include <stdio.h>

// @@@ move to armconfig.h
#define ERR_LEDCTL        (1<<0)
#define ERR_ACCEL         (1<<1)
#define ERR_EVENTS        (1<<2)
#define ERR_TXOVERFLOW    (1<<3)
#define ERR_EVENTSLOW     (1<<4)
#define ERR_BOOT          (1<<5)
#define ERR_SPURINT       (1<<6)
#define ERR_SSC_OVERFLOW  (1<<7)
#define ERR_SPI_OVERFLOW  (1<<8)
#define ERR_ADC_SPI       (1<<9)
#define ERR_SELF_TEST     (1<<10)
#define ERR_SER_PARITY    (1<<11)
#define ERR_SER_OVRRUN    (1<<12)
#define ERR_SER_FRAME     (1<<13)
#define ERR_MOTOR_ALIGN   (1<<14)
#define ERR_MAXERR      14


extern unsigned int error_reg;

/** Sets the error register. Each bit corresponds to a different driver. */
void error_set(unsigned int errflag);

void error_clear(unsigned int errflag);

unsigned int error_get ();

int error_event(void);

#endif /* LIB_ERROR_H */
