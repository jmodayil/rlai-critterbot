/*
 *  lib_error.h
 *  
 *
 *  Created by Marc G. Bellemare on 21/07/08.
 *
 *  Error-reporting driver.
 */

#include "lib_ledctl.h"

#ifndef LIB_ERROR_H
#define LIB_ERROR_H

// @@@ move to armconfig.h
#define ERR_LEDCTL      (1<<0)
#define ERR_ACCEL       (1<<1)
#define ERR_EVENTS      (1<<2)
#define ERR_TXOVERFLOW  (1<<3)
#define ERR_EVENTSLOW   (1<<4)
#define ERR_MAXERR      15

unsigned int error_reg;

/** Sets the error register. Each bit corresponds to a different driver. */
void error_set(unsigned int errflag);

void error_clear(unsigned int errflag);

unsigned int error_get ();

void error_disp(void);

#endif /* LIB_ERROR_H */
