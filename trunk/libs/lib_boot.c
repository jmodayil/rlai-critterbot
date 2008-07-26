/*
 *  lib_boot.c
 *  
 *
 *  Created by Marc G. Bellemare on 26/07/08
 *
 */

#include "compiler.h"
#include "lib_boot.h"
#include "lib_error.h"
#include "armio.h"

#include "lib_ledctl.h"

BOOT_COPY_SECTION unsigned char boot_data[BOOT_MAX_CODE_SIZE];
unsigned int boot_data_head;
unsigned int boot_data_size;
volatile unsigned int boot_receiving = 0;
unsigned int boot_timeout_counter;

/** DO NOT CALL THIS FUNCTION */
ARM_CODE RAMFUNC void boot_core()
{
  // Disable ALL interrupts
  // @@@ use Mike's code instead
  AT91C_BASE_AIC->AIC_IDCR = ~0x0;
  // @@@ Disable motors, etc
  // @@@ Enable watchdog - don't take too long; reset watchdog while copying
  // Switch to supervisor mode?
  // Copy to flash in 256 bytes blocks
  // Reset
}

void boot_reset_arm()
{
  // Tell the reset controller to reset
  AT91C_BASE_RSTC->RSTC_RCR = BOOT_RESET_SETTINGS;
  // Wait for software reset to happen
  while (!(AT91C_BASE_RSTC->RSTC_RSR & AT91C_RSTC_RSTTYP_SOFTWARE))
    ;
}

void boot_verify()
{
  // Compares the code
  // Set LED's blue channel if verified, green otherwise?
  
  // we can also printf the number of different bytes
  boot_abort_receive();
}

/**
  * Event handler for the bootloader driver.
  */
void boot_event()
{
  int val;

  if (!boot_receiving) return;

  boot_timeout_counter++;
  if (boot_timeout_counter >= BOOT_RECEIVE_TIMEOUT)
  {
    // Copy to flash! this will never return (or shouldn't)
    if (boot_data_head == boot_data_size)
      // @@@ replace with boot_core once ready
      boot_verify();
    
    // If done receiving... (or returned from boot_core??)
    error_set(ERR_BOOT);
    boot_abort_receive();
  }

  // Get at least one character
  val = armgetchar();
  if (val == EOF) return;
  // Reset the timeout counter
  boot_timeout_counter = 0;

  do
  {
    // Hmm, more data than necessary!
    if (boot_data_head >= boot_data_size || 
      boot_data_head == BOOT_MAX_CODE_SIZE)
      {
        error_set(ERR_BOOT);
        boot_abort_receive();
        return;
      }
    // Store byte in buffer
    boot_data[boot_data_head++] = (unsigned char)val;
    // Read next byte
    val = armgetchar();
  }
  while (val != EOF);
}

/**
  * Begins the reception
  */
void boot_begin_receive(int data_size)
{
  boot_receiving = 1;
  boot_data_head = 0;
  boot_data_size = data_size;
  boot_timeout_counter = 0;

  // @@@ disable UI
}

/**
  * Mayday! Abort the reception of new code
  */
void boot_abort_receive()
{
  boot_receiving = 0;
  // @@@ enable UI
}


