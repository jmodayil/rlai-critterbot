
#include "lib_flash.h"

/*
 * We shouldn't be locking things, so we _shouldn't_ need to unlock them
 */
int flash_clear_lock_bits( void ) {
  return 1;
}

/*
 * Writes the data from the page buffer to the selected flash page.
 * Blocking while write occurs.
 * 
 * returns:
 *  0 on success
 *  1 if page is out of range
 *  the value of the flash status register if the write fails, >1
 */
int flash_erase_write_page( unsigned int page ) {
  
  AT91PS_MC mc = AT91C_BASE_MC;
  unsigned int fsr;
  
  if(0 > page || AT91C_IFLASH_NB_OF_PAGES <= page)
    return AT91C_MC_PROGE;
  
  mc->MC_FMR = FLASH_FMCN << 16;
  while(!(mc->MC_FSR & AT91C_MC_FRDY));
  mc->MC_FMR = AT91C_MC_FCMD_START_PROG | 
    ( page << 8 ) | FLASH_KEY;
  do{
    fsr = mc->MC_FSR;
    if((fsr & AT91C_MC_LOCKE) | (fsr & AT91C_MC_PROGE))
      return fsr;
  }while(!(fsr & AT91C_MC_FRDY));
 
  return AT91C_MC_FRDY;
}

int flash_write_data (int * dst, int * src, unsigned int len)
{
  int i;

  // Some internal checks; len must be 1..FLASH_PAGE_WORD_SIZE
  if (len < 1 || len > FLASH_PAGE_WORD_SIZE)
    return 0;

  if ((char*) dst < AT91C_IFLASH || 
    (char*) (dst + len) > (AT91C_IFLASH + AT91C_IFLASH_SIZE))
      return 0;

  for (i = 0; i < len; i++)
    *dst++ = *src++;

  return 1;
}
