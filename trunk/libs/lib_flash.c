
#include "lib_flash.h"

int flash_erase_write_page( unsigned int page ) {
  
  AT91_REG mc = AT91C_BASE_MC;
  unsigned int fsr;
  
  if(0 > new_page || AT91C_IFLASH_NB_OF_PAGES <= new_page)
    return 1;
  
  mc->AT91C_MC_FMR = FLASH_FMCN << 16;
  while(!(mc->AT91C_MC_FSR & AT91C_MC_FRDY));
  mc->AT91C_MC_FCR = AT91C_MC_FCMD_START_PROG | 
    ( page << 8 ) | AT91C_MC_KEY;
  do{
    fsr = mc->AT91C_MC_FSR;
    if((fsr & AT91C_MC_LOCKE) | (fsr & AT91C_MC_PROGE))
      return fsr;
  }while(!(fsr & AT91C_MC_FRDY));
  
}

FLASH_FMCN
AT91C_IFLASH_PAGE_SIZE
