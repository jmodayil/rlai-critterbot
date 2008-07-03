#include "include/compiler.h"
#include "include/AT91SAM7S256.h"
#include "include/lib_AT91SAM7S256.h"

#define BUF_SIZE 128

int main() {

  char msg[BUF_SIZE];
  int i;
  
  AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, 1 << AT91C_ID_US0 );
  AT91F_PIO_CfgPeriph( AT91C_BASE_PIOA, AT91C_PA5_RXD0 | AT91C_PA6_TXD0, 0 );
  // 115200 baud, even parity
  //AT91C_BASE_DBGU->DBGU_BRGR = BAUD_CD;
  AT91C_BASE_US0->US_BRGR = 13;
  AT91C_BASE_US0->US_MR = AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE;
  // Enable TX and RX, clear status flags
  AT91C_BASE_US0->US_CR = AT91C_US_RXEN | AT91C_US_TXEN | AT91C_US_RSTSTA; 
  
  while(1) {
    
    for(i = 0; i < BUF_SIZE; i++ ) {
      while(!(AT91C_BASE_US0->US_CSR & 0x00000001));
      if( (msg[i] = (char)AT91C_BASE_US0->US_RHR) == '\n')
        break;
    }
    msg[BUF_SIZE-1] = '\n';
    
    for(i = 0; msg[i] != '\n'; i++) {
      AT91C_BASE_US0->US_THR = msg[i];
      while(!(AT91C_BASE_US0->US_CSR & 0x00000002));
    }
    AT91C_BASE_US0->US_THR = '\r';
    while(!(AT91C_BASE_US0->US_CSR & 0x00000002));
    AT91C_BASE_US0->US_THR = '\n';
    while(!(AT91C_BASE_US0->US_CSR & 0x00000002));
    
  }
}

