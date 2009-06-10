#include "armio.h"
#include "lib_error.h"
#include "lib_except.h"

RAMFUNC void spur_isr() {
  __armputchar('?');
  error_set(ERR_SPURINT);
}

void C_Undef_Handler( unsigned int calling_addr ) {
  AT91C_BASE_AIC->AIC_SPU = (unsigned int)spur_isr;
  // Disable PDC so the follow text isn't garbled.
  AT91C_BASE_US0->US_PTCR = 0x00000202;
  __armprintf("\r*******\rUndefined Instruction\r*******\r");
  __armprintf("Calling address: %p\r", calling_addr);
  while(1);
}

void C_PAbt_Handler( unsigned int calling_addr ) {
  // Disable PDC so the follow text isn't garbled.
  AT91C_BASE_US0->US_PTCR = 0x00000202;

  __armprintf("\r*******\rPrefetch Abort\r*******\r");
  __armprintf("Calling address: %p\r", calling_addr);
  __armprintf("ASR: %x\r", *AT91C_MC_ASR);
  __armprintf("AASR: %p\r", *AT91C_MC_AASR);
  while(1);
}

void C_DAbt_Handler( unsigned int calling_addr ) {
  // Disable PDC so the follow text isn't garbled.
  AT91C_BASE_US0->US_PTCR = 0x00000202;

  __armprintf("\r*******\rData Abort\r*******\r");
  __armprintf("Calling address: %p\r", calling_addr);
  __armprintf("ASR: %x\r", *AT91C_MC_ASR);
  __armprintf("AASR: %p\r", *AT91C_MC_AASR);
  while(1); 
}

void enable_watchdog( void ) {
  
  *AT91C_WDTC_WDMR = WDT_WDV | AT91C_WDTC_WDRSTEN | WDT_WDD |
    AT91C_WDTC_WDDBGHLT | AT91C_WDTC_WDIDLEHLT; 
}

void ping_watchdog( void ) {

  *AT91C_WDTC_WDCR = WDT_KEY | AT91C_WDTC_WDRSTT;

} 
