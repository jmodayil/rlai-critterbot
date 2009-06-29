#ifndef LIB_TWI_H
#define LIB_TWI_H

#include "compiler.h"
// Initialize the TWI interface for use with temp sensors
void init_twi( void );

// Read a word from the TWI bus
unsigned short twi_read_word( unsigned char bus_addr, unsigned char dev_addr );

// TWI Polling main loop
int twi_main( void );

// TWI Interrupt
ARM_CODE RAMFUNC void twi_isr();

#endif
