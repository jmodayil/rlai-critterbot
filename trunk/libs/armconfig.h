/*
 *  armconfig.h
 *  
 *  Created by Marc G. Bellemare on 7/07/2008.
 *
 *
 *  Contains some basic definitions pertaining to the particular 
 *  configuration for the ARMs.
 *
 */

#ifndef ARM_CONFIG_H
#define ARM_CONFIG_H

#define MCK 47923200

/** PIN definitions */
// I/O lines connected to the BLANK/XLAT inputs of the LED controllers
#define LEDCTL_PIN_BLANK 4
#define LEDCTL_PIN_XLAT 5
// @@@ This pin needs to be defined
#define LEDCTL_PIN_XERR -1


#endif /* ARM_CONFIG_H */
