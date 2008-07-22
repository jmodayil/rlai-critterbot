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

// @@@ move to armconfig.h
#define ID_LEDCTL 0
#define ID_ACCEL  1
#define ID_EVENTS 2

/** Sets the error register. Each bit corresponds to a different driver. */
void error_set(unsigned int errflag);

unsigned int error_get ();

#endif /* LIB_ERROR_H */
