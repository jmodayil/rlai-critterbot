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
#define LEDCTL_PIN_BLANK 9
#define LEDCTL_PIN_XLAT 26
#define LEDCTL_PIN_XERR 25
#define LEDCTL_PIN_MODE 15

// Function to initialized on-chip settings on power-up
void chip_init(void);

#endif /* ARM_CONFIG_H */
