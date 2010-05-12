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

#define SOUND_PIN_AMP_ENABLE 8

/** Bootloader flags */
/* Only one of these should be #defined
 * If using BOOT_LOAD_TO_RAM_AND_COPY, the code will be stored in RAM when
 * read from the serial port, then copied to flash. It is safer (partial
 * data transfers will fail), but cannot accodomate code bigger than 40k,
 * most likely.
 *
 * BOOT_LOAD_TO_FLASH will copy mostly directly to flash.
 *
 */
#define BOOT_LOAD_TO_RAM_AND_COPY
// #define BOOT_LOAD_TO_FLASH

// Function to initialized on-chip settings on power-up
void chip_init(void);

extern void crit_enable_int();
extern void crit_disable_int();

#endif /* ARM_CONFIG_H */
