/*
 *
 *  Created by Mike Sokolsky on 24/07/08
 *
 */

#include "AT91SAM7S256.h"

#ifndef LIB_FLASH_H
#define LIB_FLASH_H

#include "armconfig.h"
#include "compiler.h"
#include "AT91SAM7S256.h"

// Number of master clock cycles in 1.5uS, rounded up
#define FLASH_FMCN (MCK / 666667 + 1)
#define FLASH_KEY  (0xA5 << 24)
#define FLASH_PAGE_WORD_SIZE (AT91C_IFLASH_PAGE_SIZE / 4)

/*
 * We shouldn't be locking things, so we _shouldn't_ need to unlock them
 */
int flash_clear_lock_bits( void );

/*
 * Writes the data from the page buffer to the selected flash page.
 * Blocking while write occurs.
 *
 * returns:
 *  0 on success
 *  1 if the page is out of range
 *  the value of the flash status register if the write fails, >1
 */
int flash_erase_write_page( unsigned int );

/*
 *  Writes to the flash, reading from src and copying to dst.
 *  dst should be between AT91C_IFLASH and AT91C_IFLASH_SIZE
 *  len should be between 1 and AT91C_IFLASH_PAGE_SIZE/4
 *   len is in words
 *
 *  Returns 0 on error, 1 otherwise.
 */
int flash_write_data (int * dst, int * src, unsigned int len);

#endif


