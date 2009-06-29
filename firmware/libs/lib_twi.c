
#include "AT91SAM7S256.h"
#include "lib_AT91SAM7S256.h"
#include "lib_twi.h"
#include "armio.h"
#include <stdio.h>
#include <string.h>

/*static unsigned char crclookup=(0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D,
                                
                                0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D,
                                
                                0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD,
                                
                                0x90,0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD,
                                
                                0xC7,0xC0,0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA,
                                
                                0xB7,0xB0,0xB9,0xBE,0xAB,0xAC,0xA5,0xA2,0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A,
                               
                                0x*/
                                
                                
volatile unsigned int twi_flag;

ARM_CODE RAMFUNC void twi_isr(void) {
  twi_flag = AT91C_BASE_TWI->TWI_SR;
  AT91C_BASE_TWI->TWI_IDR = 0x00000107;
}

void init_twi( void )
{
  // Enable External Reset?  Remove at some future point
  *AT91C_RSTC_RMR = 0xA5000001;
  AT91F_TWI_CfgPIO();
  AT91F_TWI_CfgPMC();
  AT91F_PIO_CfgOpendrain(AT91C_BASE_PIOA, 
                          ((unsigned int) AT91C_PA3_TWD ) |
                          ((unsigned int) AT91C_PA4_TWCK ) );
  
  AT91F_AIC_ConfigureIt( AT91C_BASE_AIC, 
                         AT91C_ID_TWI,
                         0,
                         AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL,
                         (void*)twi_isr );
  AT91F_AIC_EnableIt( AT91C_BASE_AIC, AT91C_ID_TWI );
  
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_SWRST;
  AT91C_BASE_TWI->TWI_MMR = (AT91C_TWI_IADRSZ_1_BYTE | 
                             AT91C_TWI_MREAD | 0x05A0000 );
  AT91C_BASE_TWI->TWI_CWGR = 0x0002FFFF;
  AT91C_BASE_TWI->TWI_IER = 0x00000103;
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_MSEN;
}

void twi_write_word0( unsigned char bus_addr, unsigned char dev_addr, unsigned int data) {
  
  AT91C_BASE_TWI->TWI_MMR &= ~(AT91C_TWI_MREAD | AT91C_TWI_DADR);
  AT91C_BASE_TWI->TWI_MMR |= (0x00 << 16);
  AT91C_BASE_TWI->TWI_IADR = (unsigned int)0x24;
  AT91C_BASE_TWI->TWI_THR = 0x00;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000104;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK1\r");
    return;
  }
  AT91C_BASE_TWI->TWI_THR = 0x00;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000104;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK2\r");
    return;
  }
  AT91C_BASE_TWI->TWI_THR = 0xE8;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000104;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK3\r");
    return;
  }
  return;

}

void twi_write_word1( unsigned char bus_addr, unsigned char dev_addr, unsigned int data) {
  
  AT91C_BASE_TWI->TWI_MMR &= ~(AT91C_TWI_MREAD | AT91C_TWI_DADR);
  AT91C_BASE_TWI->TWI_MMR |= (0x00 << 16);
  AT91C_BASE_TWI->TWI_IADR = (unsigned int)0x24;
  AT91C_BASE_TWI->TWI_THR = 0xFF;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000104;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK1\r");
    return;
  }
  AT91C_BASE_TWI->TWI_THR = 0xFF;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000104;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK2\r");
    return;
  }
  AT91C_BASE_TWI->TWI_THR = 0xCC;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000104;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK3\r");
    return;
  }
  return;

}

void twi_write_word2( unsigned char bus_addr, unsigned char dev_addr, unsigned int data) {
  
  AT91C_BASE_TWI->TWI_MMR &= ~(AT91C_TWI_MREAD | AT91C_TWI_DADR);
  AT91C_BASE_TWI->TWI_MMR |= (0x00 << 16);
  AT91C_BASE_TWI->TWI_IADR = (unsigned int)0x2E;
  AT91C_BASE_TWI->TWI_THR = 0x00;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000104;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK1\r");
    return;
  }
  AT91C_BASE_TWI->TWI_THR = 0x00;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000104;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK2\r");
    return;
  }
  AT91C_BASE_TWI->TWI_THR = 0x6F;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000104;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK3\r");
    return;
  }
  return;

}

void twi_write_word3( unsigned char bus_addr, unsigned char dev_addr, unsigned int data) {
  
  AT91C_BASE_TWI->TWI_MMR &= ~(AT91C_TWI_MREAD | AT91C_TWI_DADR);
  AT91C_BASE_TWI->TWI_MMR |= (0x00 << 16);
  AT91C_BASE_TWI->TWI_IADR = (unsigned int)0x2E;
  AT91C_BASE_TWI->TWI_THR = 0x01;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000104;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK1\r");
    return;
  }
  AT91C_BASE_TWI->TWI_THR = 0x00;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000104;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK2\r");
    return;
  }
  AT91C_BASE_TWI->TWI_THR = 0x7A;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000104;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK3\r");
    return;
  }
  return;

}

unsigned short twi_read_word( unsigned char bus_addr, unsigned char dev_addr ) {
  
  unsigned short data = 0;
  unsigned char pec = 0;
  unsigned char crc[6], crtemp[2], crcode = 0x07;
  
  crc[0] = bus_addr << 1;
  crc[1] = dev_addr;
  crc[2] = (bus_addr << 1) | 0x1;
  crc[5] = 0x00;
  
  AT91C_BASE_TWI->TWI_MMR &= ~(AT91C_TWI_DADR | AT91C_TWI_MREAD);
  AT91C_BASE_TWI->TWI_MMR |= (AT91C_TWI_MREAD | (bus_addr << 16));
  AT91C_BASE_TWI->TWI_IADR = (unsigned int)dev_addr;
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_START;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000102;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK!\r");
    return 0;
  }
  crc[3] = AT91C_BASE_TWI->TWI_RHR;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000102;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK!\r");
    return 0;
  }
  crc[4] = AT91C_BASE_TWI->TWI_RHR;
  AT91C_BASE_TWI->TWI_CR = AT91C_TWI_STOP;
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000102;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK!\r");
    return 0;
  }
  twi_flag = 0;
  AT91C_BASE_TWI->TWI_IER = 0x00000101;
  while(twi_flag == 0);
  if( twi_flag & AT91C_TWI_NACK ) {
    armprintf("NACK!\r");
    return 0;
  }
    
  pec = AT91C_BASE_TWI->TWI_RHR;
  
  return (crc[3] | (crc[4] << 8));
}

//int twi_crc_check( unsigned char* crc ) {
//
//}

int twi_main()
{
  unsigned int data = 0;
  unsigned int object, ambient;
  unsigned char addr1 = 0x03;
  unsigned char addr2 = 0x04;

  /*armprintf("Press ENTER to set EMISSIVITY\r");
  while(armgetchar()==EOF);
  twi_write_word0(0,0,0);
  armprintf("Press ENTER to confirm\r");
  while(armgetchar()==EOF);
  twi_write_word1(0,0,0);
  armprintf("Press ENTER to set Address\r");
  while(armgetchar()==EOF);
  twi_write_word2(0,0,0);
  armprintf("Press ENTER to confirm\r");
  while(armgetchar()==EOF);
  twi_write_word3(0,0,0);
  return 0;*/
  while(1) {
    armprintf("Press ENTER to measure...\r");
    while(armgetchar()==EOF);
    data = twi_read_word( addr1, 0x06 );
    armprintf("SENSOR 3------------------\r");
    armprintf("Ambient Raw: %x\r", data);
    ambient = (unsigned int)(((float)data * 100) * 0.02) - 27315;
    armprintf("Ambient Temp: %d.%d C\r", ambient, data);
    //while(armgetchar()==EOF);
    data = twi_read_word( addr1, 0x07 );
    armprintf("Zone 1 Raw: %x\r", data);
    object = (unsigned int)(((float)data * 100) * 0.02) - 27315;
    armprintf("Zone 1 Temp: %d.%d C\r", object, data);
    //data = twi_read_word( 0x5A, 0x06 );
    data = twi_read_word( addr1, 0x08 );
    armprintf("Zone 2 Raw: %x\r", data);
    object = (unsigned int)(((float)data * 100) * 0.02) - 27315;
    armprintf("Zone 2 Temp: %d.%d C\r", object, data);
    armprintf("SENSOR 4------------------\r");
    data = twi_read_word( addr2, 0x06 );
    armprintf("Ambient Raw: %x\r", data);
    ambient = (unsigned int)(((float)data * 100) * 0.02) - 27315;
    armprintf("Ambient Temp: %d.%d C\r", ambient, data);
    //while(armgetchar()==EOF);
    data = twi_read_word( addr2, 0x07 );
    armprintf("Zone 1 Raw: %x\r", data);
    object = (unsigned int)(((float)data * 100) * 0.02) - 27315;
    armprintf("Zone 1 Temp: %d.%d C\r", object, data);
    //data = twi_read_word( 0x5A, 0x06 );
    data = twi_read_word( addr2, 0x08 );
    armprintf("Zone 2 Raw: %x\r", data);
    object = (unsigned int)(((float)data * 100) * 0.02) - 27315;
    armprintf("Zone 2 Temp: %d.%d C\r", object, data);
    /*data = twi_read_word( addr, 0x20 );
    armprintf("ToMax: %x\r", data);
    data = twi_read_word( addr, 0x21 );
    armprintf("ToMin: %x\r", data);
    data = twi_read_word( addr, 0x22 );
    armprintf("PWMCTRL: %x\r", data);
    data = twi_read_word( addr, 0x23 );
    armprintf("TaRange: %x\r", data);
    data = twi_read_word( addr, 0x24 );
    armprintf("Emissivity: %x\r", data);
    data = twi_read_word( addr, 0x25 );
    armprintf("Config1: %x\r", data);
    data = twi_read_word( addr, 0x2E );
    armprintf("Address: %x\r", data);
    data = twi_read_word( addr, 0x3C );
    armprintf("ID1: %x\r", data);
    data = twi_read_word( addr, 0x3D );
    armprintf("ID2: %x\r", data);
    data = twi_read_word( addr, 0x3E );
    armprintf("ID3: %x\r", data);
    data = twi_read_word( addr, 0x3F );
    armprintf("ID4: %x\r", data);
*/
  }
  return 0;
}//* End
