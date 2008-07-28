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
  // Disable interrupts
  asm volatile("mrs r0, cpsr\n\t"
      "orr r0, r0, #0x80\n\t"
      "msr cpsr_c, r0"
      : : : "r0" );
  // @@@ Disable motors, etc
  // @@@ Enable watchdog - don't take too long; reset watchdog while copying
  // Switch to supervisor mode?
  // Copy to flash in 256 bytes blocks
  // Reset - without re-enabling interrupts!
  boot_reset_arm();
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
  unsigned int num_diff_bytes = 0;
  unsigned int num_data_words = boot_data_size / 4;
  unsigned int num_data_tail = boot_data_size % 4;
  unsigned int * source = (unsigned int*)(boot_data);
  unsigned int * dest = (unsigned int*)(BOOT_COPY_DESTINATION);
  int i;
  
  // Compares the code; assumes binary is word-aligned
  for (i = 0; i < num_data_words; i++)
    if (*source++ != *dest++)
      num_diff_bytes += 4;
 
  // Now compare the tail
  for (i = 0; i < num_data_tail; i++)
  {
    if (*(((unsigned char *)source)+i) != *(((unsigned char *)dest)+i))
      num_diff_bytes++;
  }
  
  // Set LED's blue channel if verified, green otherwise
  if (num_diff_bytes == 0)
  {
    armprintf ("Binary verified.\r");
    for (i = 0; i < LEDCTL_NUM_LEDS; i++)
      ledctl_setvalue(BLUE_CONTROLLER, i, 512);
  }
  else
  {
    armprintf ("Binaries differ in %u (%u) bytes.\r", num_diff_bytes, 
      boot_data_size);
    for (i = 0; i < LEDCTL_NUM_LEDS; i++)
      ledctl_setvalue(GREEN_CONTROLLER, i, 512);
  }
  
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
    {
      // @@@ replace with boot_core once ready
      armprintf ("Begin verify.\r");
      boot_verify();
      armprintf ("End verify.\r");
    }
    
    armprintf ("Timeout!\r");
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

// @@@ take me out
extern int run_ui;

/**
  * Begins the reception
  */
void boot_begin_receive(int data_size)
{
  boot_receiving = 1;
  boot_data_head = 0;
  boot_data_size = data_size;
  boot_timeout_counter = 0;

  run_ui = 0;
  // @@@ disable UI
}

/**
  * Mayday! Abort the reception of new code
  */
void boot_abort_receive()
{
  boot_receiving = 0;
  run_ui = 1;
  // @@@ enable UI
}


