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
#include "lib_flash.h"
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
  unsigned int num_pages;
  unsigned int tail_page_size;
  unsigned int i;

  unsigned char * source;
  unsigned char * dest;

  int errflag = 0;

  // Disable interrupts
  asm volatile("mrs r0, cpsr\n\t"
      "orr r0, r0, #0x80\n\t"
      "msr cpsr_c, r0"
      : : : "r0" );
  // @@@ Disable motors, etc
  // @@@ Enable watchdog - don't take too long; reset watchdog while copying
  // Switch to supervisor mode?
  num_pages = boot_data_size / AT91C_IFLASH_PAGE_SIZE;
  tail_page_size = boot_data_size % AT91C_IFLASH_PAGE_SIZE;

  // The data size should be a multiple of 4! Pad the end with zeros
  tail_page_size += 4 - (tail_page_size & (sizeof(int)-1));
  
  source = (unsigned char*)BOOT_BUFFER;
  dest = (unsigned char*)BOOT_COPY_DESTINATION;

  // Copy all pages except possibly the last one
  for (i = 0; i < num_pages; i++)
  {
    // Write a full page (length is in words)
    if (!flash_write_data((int *) dest, (int *)source, 
      AT91C_IFLASH_PAGE_SIZE / 4))
      errflag = 1;
    if (flash_erase_write_page (i) != AT91C_MC_FRDY)  
      errflag = 1;

    if (errflag) break;

    dest += AT91C_IFLASH_PAGE_SIZE;
    source += AT91C_IFLASH_PAGE_SIZE;
  }

  // Write the last page, if necessary
  if (tail_page_size > 0)
  {
    // We know tail_page_size % 4 == 0 because we set it to be so above
    if (!flash_write_data((int *) dest, (int *)source,
      tail_page_size / 4))
      errflag = 1;
    if (flash_erase_write_page(i) != AT91C_MC_FRDY)
      errflag = 1;

    dest += tail_page_size;
    source += tail_page_size;
  }
  
  // Reset - without re-enabling interrupts!
  if (!errflag)
    boot_reset_arm();
  else
  {
    // Write to the serial port and die (should be English pound sign)
    AT91C_BASE_US0->US_THR = 163;
    while (1) ;
  }
}

void boot_reset_arm()
{
  // Tell the reset controller to reset
  AT91C_BASE_RSTC->RSTC_RCR = BOOT_RESET_SETTINGS;
  
  
  // Wait for software reset to happen
  while (!(AT91C_BASE_RSTC->RSTC_RSR & AT91C_RSTC_RSTTYP_SOFTWARE))
    armprintf(".")
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
    armprintf("\r");
    // Copy to flash! this will never return (or shouldn't)
    if (boot_data_head == boot_data_size)
    {
      // @@@ replace with boot_core once ready
      armprintf ("Begin verify.\r");
      boot_verify();
      armprintf ("End verify.\r");
    }
    else if (boot_data_head > boot_data_size) {
     armprintf ("Got %d bytes too much data!\r", boot_data_head - boot_data_size);
    } else {
      armprintf ("Timeout!\r");
      // If done receiving... (or returned from boot_core??)
      error_set(ERR_BOOT);
    }
    boot_abort_receive();
  }

  // Get at least one character
  val = armgetchar();
  if (val == EOF) return;
  // Reset the timeout counter
  boot_timeout_counter = 0;
  do
  {
    if(boot_data_head % 1024 == 0)
      armputchar('.');
    // Hmm, more data than necessary!
    if (boot_data_head >= boot_data_size || 
      boot_data_head == BOOT_MAX_CODE_SIZE)
      {
        error_set(ERR_BOOT);
        //boot_abort_receive();
        //return;
        boot_data_head++;
      }
    else {// Store byte in buffer
      boot_data[boot_data_head++] = (unsigned char)val;
    }// Read next byte
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


