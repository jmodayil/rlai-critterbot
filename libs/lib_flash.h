/*
 *
 *  Created by Mike Sokolsky on 24/07/08
 *
 */

#include "AT91SAM7S256.h"

#ifndef LIB_FLASH_H
#define LIB_FLASH_H


// Number of master clock cycles in 1.5uS
#define FLASH_FMCN (MCK / 666667)

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


#endif


