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
#include "lib_critical.h"
#include "lib_ledctl.h"
#include "lib_leddrive.h"
#include "lib_events.h"
#include "crctable.h"
#include "AT91SAM7S256.h"

event_s boot_event_s = {
  NULL,
  boot_event,
  0
};

BOOT_COPY_SECTION unsigned char boot_data[BOOT_MAX_CODE_SIZE];
unsigned int boot_data_head;
unsigned int boot_data_size;
volatile unsigned int boot_receiving = 0;
unsigned int boot_timeout_counter;
int crc_was_good;

/** DO NOT CALL THIS FUNCTION */
ARM_CODE RAMFUNC void boot_core()
{
  unsigned int num_pages;
  unsigned int tail_page_size;
  unsigned int i;

  unsigned char * source;
  unsigned char * dest;

  int errflag = 0;

  //remove
  int tmp = 0;

  if(0 == crc_was_good) {
    armprintf("No valid boot image to flash!\r");
    return;
  }
  // Disable interrupts
  crit_disable_int();
  // @@@ Disable motors, etc
  // Switch to supervisor mode?
  // We don't need the CRC in flash
  boot_data_size -= 2;
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
      AT91C_IFLASH_PAGE_SIZE / 4)) {
      errflag = 1;
    }
    if ((tmp = flash_erase_write_page (i)) != 0) {
      errflag = 1;
    }

    if( i % 80 == 0)
      __armputchar('\r');
    __armputchar('.');
    if (errflag) break;

    dest += AT91C_IFLASH_PAGE_SIZE;
    source += AT91C_IFLASH_PAGE_SIZE;
  }

  // Write the last page, if necessary
  if (!errflag && tail_page_size > 0)
  {
    // We know tail_page_size % 4 == 0 because we set it to be so above
    if (!flash_write_data((int *) dest, (int *)source,
      tail_page_size / 4)) {
      errflag = 1;
    }
    if (flash_erase_write_page(i) != 0) {
      errflag = 1;
    }

    dest += tail_page_size;
    source += tail_page_size;
  }
  
  // Reset - without re-enabling interrupts!
  if (!errflag)
    boot_reset_arm();
  else
  {
		// No particular reason to think anything will still be running if we get here,
		// but just in case...
    __armputchar('\r'); 
    __armputchar('F');
    __armputchar('L');
    __armputchar('A');
    __armputchar('S');
    __armputchar('H');
    __armputchar(' ');
    __armputchar('F');
    __armputchar('A');
    __armputchar('I');
    __armputchar('L');
    __armputchar('E');
    __armputchar('D');
    __armputchar('\r');
    while (1);
  }
}

ARM_CODE RAMFUNC void boot_reset_arm()
{
  // Tell the reset controller to reset
  AT91C_BASE_RSTC->RSTC_RCR = BOOT_RESET_SETTINGS;
  
  
  // Wait for software reset to happen
  while (!(AT91C_BASE_RSTC->RSTC_RSR & AT91C_RSTC_RSTTYP_SOFTWARE));
}

void boot_verify()
{
  
  unsigned short my_crc, file_crc;

  my_crc = get_crc( boot_data, boot_data_size - 2);
  file_crc = boot_data[boot_data_size - 2] << 8;
  file_crc += boot_data[boot_data_size - 1];
  if(my_crc == file_crc) {
    armprintf("CRC Check OK.  Ready to flash.\r");
    crc_was_good = 1; 
  }
  else {
    armprintf("CRC Check FAILED.  Re-send file.\r");
    crc_was_good = 0;
  }
}

unsigned short get_crc( unsigned char *data, int len ) {

 unsigned short reg;

 reg = 0;
 while(len--)
   reg = (reg<<8) ^ crctable[(reg >> 8) ^ *data++];
 return reg;
}


/**
  * Event handler for the bootloader driver.
  */
int boot_event()
{
  int val;
  if (!boot_receiving) return 1;

  boot_timeout_counter++;
  if (boot_timeout_counter >= BOOT_RECEIVE_TIMEOUT)
  {
    // If we got the right amount of data, verify with CRC.
    if (boot_data_head == boot_data_size)
      boot_verify();
    else if (boot_data_head > boot_data_size) {
     armprintf ("Got %d bytes too much data.\r", boot_data_head - boot_data_size);
    } else {
      armprintf ("Timeout.\r");
      // If done receiving... 
      error_set(ERR_BOOT);
    }
    boot_end_receive();
    return 0;
  }

  // Get at least one character
  val = armgetchar();
  if (val == EOF) return 1;
  // Reset the timeout counter
  boot_timeout_counter = 0;
  do
  {
    // Hmm, more data than necessary!
    if (boot_data_head >= boot_data_size || 
      boot_data_head == BOOT_MAX_CODE_SIZE)
      {
        error_set(ERR_BOOT);
        boot_data_head++;
      }
    else {// Store byte in buffer
      boot_data[boot_data_head++] = (unsigned char)val;
    }// Read next byte
    val = armgetchar();
  }
  while (val != EOF);
  return 0;
}

/**
  * Begins the reception
  */
void boot_begin_receive(int data_size)
{
  boot_receiving = 1;
  boot_data_head = 0;
  boot_data_size = data_size + 2;
  boot_timeout_counter = 0;
  crc_was_good = 0;
  leddrive_busy();
  event_start(EVENT_ID_BOOT);
  event_stop(EVENT_ID_UI);
}

/**
  * Ends reception of code (for whatever reason)
  */
void boot_end_receive()
{
  leddrive_ui();
  boot_receiving = 0;
  event_stop(EVENT_ID_BOOT);
  event_start(EVENT_ID_UI);
}

int get_reset_code()
{
  return (AT91C_BASE_RSTC->RSTC_RSR & AT91C_RSTC_RSTTYP) >> 8;
}


