#ifndef LIB_EXCEPT_H
#define LIB_EXCEPT_H

RAMFUNC void spur_isr(); 

void C_Undef_Handler( unsigned int calling_addr );

void C_PAbt_Handler( unsigned int calling_addr );

void C_DAbt_Handler( unsigned int calling_addr );

void enable_watchdog( void );

void ping_watchdog( void ); 

#define WDT_KEY 0xA5000000

// in 1/256th of a second
// MGB: Modified to follow Errata 40.4.14.1 
#define WDT_WDV 0x00F

// MGB: Modified to follow Errata 40.4.14.1 
#define WDT_WDD ((0xFFF) << 16)

#endif //LIB_EXCEPT_H
