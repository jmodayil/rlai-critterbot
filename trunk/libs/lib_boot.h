/*
 *  lib_boot.h
 *  
 *
 *  Created by Marc G. Bellemare on 26/07/08
 *
 *  Bootloader driver.
 */

#ifndef LIB_BOOT_H
#define LIB_BOOT_H

#include "compiler.h"
#include "armconfig.h"
#include "AT91SAM7S256.h"

#ifdef BOOT_LOAD_TO_RAM_AND_COPY
// This definition MUST match the linker script's BOOTRAM size!
#define BOOT_MAX_CODE_SIZE  0xA000
// @@@ fixme
#define BOOT_BUFFER         (boot_data)
#elif defined(BOOT_LOAD_TO_FLASH)
#define BOOT_MAX_CODE_SIZE AT91C_IFLASH_SIZE
#define BOOT_BUFFER        (AT91C_IFLASH + AT91C_IFLASH_SIZE/2)
#else
#error No bootloader mode set.
#endif

#define BOOT_COPY_DESTINATION AT91C_IFLASH

#define BOOT_COPY_SECTION   __attribute__ ((section (".bootcopy")))

// How long to wait before giving up on transfer, in 100Hz (event-driven)
#define BOOT_RECEIVE_TIMEOUT  100

#define BOOT_PASSWORD "cookie"

#define RESET_KEY (0xA5000000)

/**
  * Two possible flags can be set here, AT91C_RSTC_PROCRST and 
  * AT91C_RSTC_PERRST. The first one resets the processor and the watchdog
  * and should always be present to result in a proper reset.
  * The second resets the peripherals; documentation suggests it only needs
  * to be used for debugging purposes.
  */
#define BOOT_RESET_SETTINGS (AT91C_RSTC_PROCRST | AT91C_RSTC_PERRST | RESET_KEY)

extern unsigned char boot_data[];
extern unsigned int boot_data_head;
extern unsigned int boot_data_size;
extern volatile unsigned int boot_receiving;
extern unsigned int boot_timeout_counter;

/** Do NOT call this function. Let the machine do the job. This function
  * copies the code to the flash and resets. */
ARM_CODE RAMFUNC void boot_core();

/**
  * After receiving a new binary, compares it with the current binary
  * Sets all LEDs to blue in response
  */
void boot_verify();

/*
 * CRC-16 function
 */ 
unsigned short get_crc( unsigned char *data, int len ); 

/**
  * Event handler for the bootloader driver.
  */
int boot_event();

/**
  * Begins the reception
  */
void boot_begin_receive(int data_size);

/**
  * Mayday! Abort the reception of new code
  */
void boot_end_receive();

/**
  * This resets the ARM. You might want to call it from somewhere else,
  * but expect harsh consequences (e.g., a reboot).
  */
ARM_CODE RAMFUNC void boot_reset_arm();


/*
 * Returns the cause of the last chip reset.
 * 0: Power-up Reset
 * 1: Watchdog Fault
 * 2: Software Reset
 * 3: External NRST
 * 4: Brownout Reset
 */
int get_reset_code();
#endif /* LIB_BOOT_H */
