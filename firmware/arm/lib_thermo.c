#include "lib_thermo.h"
#include "lib_error.h"
#include "lib_events.h"

event_s thermo_event_s = {
  thermo_init,
  thermo_event,
  0
};

unsigned char thermo_command[THERMO_NUM_THERMS][THERMO_COMMAND_SIZE];
unsigned char thermo_vals[THERMO_NUM_THERMS][THERMO_VAL_SIZE];
int thermo_num, thermo_val_index;
volatile int thermo_int;

void thermo_twi_init(void) {
  AT91C_BASE_TWI->TWI_IER = 0x00000000;
  AT91F_TWI_CfgPIO();
  AT91F_TWI_CfgPMC();
  AT91F_PIO_CfgOpendrain(AT91C_BASE_PIOA, 
                          ((unsigned int) AT91C_PA3_TWD ) |
                          ((unsigned int) AT91C_PA4_TWCK ) );
  
  AT91F_AIC_ConfigureIt( AT91C_BASE_AIC, 
                         AT91C_ID_TWI,
                         0,
                         AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL,
                         (void*)thermo_isr );
  AT91F_AIC_EnableIt( AT91C_BASE_AIC, AT91C_ID_TWI );
  
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_SWRST;
  AT91C_BASE_TWI->TWI_MMR = (AT91C_TWI_IADRSZ_1_BYTE | 
                             AT91C_TWI_MREAD );
  // Clock rate at 92kHz
  AT91C_BASE_TWI->TWI_CWGR = 0x0001C0C0;
  //AT91C_BASE_TWI->TWI_CWGR = 0x0002C0C0;
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_MSEN | 0x20;
}

void thermo_command_init(void) {
int i;

//for(i = 0; i < THERMO_NUM_THERMS; i+=2) {
  /*thermo_command[i][0] = ((i+1)>>2) + 1;
  thermo_command[i][1] = 0x07;
  thermo_command[i+1][0] = ((i+1)>>2) + 1;
  thermo_command[i+1][1] = 0x08;*/
  thermo_command[0][0] = 1;
  thermo_command[0][1] = 0x07;
  thermo_command[1][0] = 1;
  thermo_command[1][1] = 0x08;
  thermo_command[2][0] = 2;
  thermo_command[2][1] = 0x07;
  thermo_command[3][0] = 2;
  thermo_command[3][1] = 0x08;
  thermo_command[4][0] = 3;
  thermo_command[4][1] = 0x07;
  thermo_command[5][0] = 3;
  thermo_command[5][1] = 0x08;
  thermo_command[6][0] = 4;
  thermo_command[6][1] = 0x07;
  thermo_command[7][0] = 4;
  thermo_command[7][1] = 0x08;
//}
}

int thermo_init(void) {
  thermo_twi_init();
  thermo_command_init();
  thermo_num = 8;
  thermo_int = 1;
  return 0;
}

int thermo_event(void) {
  thermo_num = 0; 
  if(thermo_int == 0) {
    thermo_twi_init();
  }
  thermo_int = 0;
  //AT91C_BASE_TWI->TWI_MMR &= AT91C_TWI_DADR;
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_MSEN | 0x20;
  AT91C_BASE_TWI->TWI_MMR = ((( unsigned int)
      (thermo_command[thermo_num][0])) << 16) | AT91C_TWI_MREAD |
      AT91C_TWI_IADRSZ_1_BYTE;
  AT91C_BASE_TWI->TWI_IADR = (unsigned int)thermo_command[thermo_num][1];
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START;
  AT91C_BASE_TWI->TWI_IER = 0x00000103;
  return 0;
}

int thermo_get_val(int i) {
if(i < 0 || i >= THERMO_NUM_THERMS)
  return 0;
return thermo_vals[i][0] | (thermo_vals[i][1] << 8);
}

ARM_CODE RAMFUNC void thermo_isr(void) {
  unsigned int twi_flags = AT91C_BASE_TWI->TWI_SR;
  if( twi_flags & AT91C_TWI_NACK ) {
    error_set(ERR_TWI_NACK);
    return;
  }
  if( twi_flags & AT91C_TWI_RXRDY ) {
    thermo_vals[thermo_num][thermo_val_index++] = AT91C_BASE_TWI->TWI_RHR;
    if(thermo_val_index == 2)
      AT91C_BASE_TWI->TWI_CR = AT91C_TWI_STOP; 
    // This should technically never be true, but just in case we leave it
    // here to prevent possible memory corruption.
    if(thermo_val_index > 2) {
      thermo_val_index = 0;
    }
  }
  if( twi_flags & AT91C_TWI_TXCOMP ) {
    if(++thermo_num < THERMO_NUM_THERMS) {
      //AT91C_BASE_TWI->TWI_MMR &= AT91C_TWI_DADR;
      AT91C_BASE_TWI->TWI_CR = AT91C_TWI_MSEN | 0x20;
      AT91C_BASE_TWI->TWI_MMR = ((( unsigned int)
          (thermo_command[thermo_num][0])) << 16) | AT91C_TWI_MREAD |
          AT91C_TWI_IADRSZ_1_BYTE;
      AT91C_BASE_TWI->TWI_IADR = (unsigned int)thermo_command[thermo_num][1];
      AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START;
      thermo_val_index = 0;
    } else {
      AT91C_BASE_TWI->TWI_IDR = 0x00000103;
      thermo_int = 1;
    }
  }
}
