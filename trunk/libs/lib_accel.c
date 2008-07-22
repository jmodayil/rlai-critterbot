/*
 *  lib_accel.c
 *  
 *
 *  Created by Marc G. Bellemare on 21/07/08.
 *
 */

#include "compiler.h"
#include "lib_accel.h"
#include "lib_spi.h"
#include "lib_error.h"

#include "armio.h"

unsigned char accel_txdata[ACCEL_BUFFER_SIZE];
unsigned char accel_rxdata[ACCEL_BUFFER_SIZE];

unsigned char accel_status;
// This data is signed!
short accel_output[ACCEL_NUM_AXES];

struct spi_packet accel_spi_packet;

/**
  * Initialization routine for the accelerometer driver.
  */
void accel_init()
{
  unsigned int val;

  // Configure the SPI packet
  accel_spi_packet.finished = 1;
  accel_spi_packet.device_id = ACCEL_SPI_DEVICE_ID;
  accel_spi_packet.data_to_write = (unsigned int*)accel_txdata;
  accel_spi_packet.read_data = (unsigned int*)accel_rxdata;
  // num words will be set on a per-call basis, but set it to 1 for safety
  accel_spi_packet.num_words = 1;

  // Write to the accelerometer's control registers

  // Power up (in little-endian)
  accel_write_reg_block(ACCEL_REG_CTRL1, ACCEL_CTRL1_POWER_LITTLE);
  // Note that there is a 31ms (5 / 160Hz) delay before the first bit of data 
  //  becomes available after we power on the accelerometer

  // Switch to big-endian
  accel_write_reg_block(ACCEL_REG_CTRL2, ACCEL_CTRL2_BIGENDIAN_LITTLE);

  // CTRL1 settings
  accel_write_reg_block(ACCEL_REG_CTRL1, ACCEL_CTRL1_SETTINGS);

  // CTRL2 settings
  accel_write_reg_block(ACCEL_REG_CTRL2, ACCEL_CTRL2_SETTINGS);

  // Sanity check - read the WHOAMI register
  val = accel_read_reg_block(ACCEL_REG_WHOAMI);
  if (val != ACCEL_WHOAMI_VALUE)
  {
    error_set (1 << ID_ACCEL);
    armprintf ("This accelerometer is a spy!\n");
    // Disable  accelerometer?
  }

}

void accel_event()
{
  int i;
  // Load accel_output and accel_status with the values requested during the
  //  last event
  // The first byte read is the status register
  accel_status = accel_rxdata[1];
 
  for (i = 0; i < ACCEL_NUM_AXES; i++)
  {
    // The accelerometer data is split into a low part (accel_rxdata[2])
    //  and a high part (accel_rxdata[3]). The high part in 12-bit mode
    //  only has 5 significant values; bits 4-6 must be thrown out.
    accel_output[i] = accel_rxdata[(i<<1)+2] | 
      ((accel_rxdata[(i<<1)+3] & 0x8F) << 8);
  }
  
  // Request status + out values for next event
  accel_read (ACCEL_REG_OUT, 7);
}

void accel_write_reg_block (unsigned int address, unsigned int data)
{
  // Wait for the spi_packet to be free
  while (accel_spi_packet.finished == 0) ;
  // Write data
  accel_write_reg (address, data);
  // Wait again
  while (accel_spi_packet.finished == 0) ;
}

/** Writes to a single register, specified by the address. */
void accel_write_reg (unsigned int address, unsigned int data)
{
  if (accel_spi_packet.finished == 0)
  {
    error_set(1 << ID_ACCEL);
    return;
  }

  // Prepare the data and the SPI packet
  accel_txdata[0] = 
    (unsigned char)((address & ACCEL_SPI_ADDR) | ACCEL_SPI_WRITE);
  accel_txdata[1] = (unsigned char)(data & 0xFF);

  accel_spi_packet.finished = 0;
  // Two words, address + flags & data
  accel_spi_packet.num_words = 2;
  
  spi_send_packet (&accel_spi_packet);
}

/** Writes to 'count' consecutive registers starting at the given address.
  * The data to be written should be placed in accel_txdata[1]...
  *  accel_txdata[count].
  */
void accel_write (unsigned int address, unsigned int count)
{
  if (accel_spi_packet.finished == 0 || count >= ACCEL_BUFFER_SIZE)
  {
    error_set(1 << ID_ACCEL);
    return;
  }

  // Prepare the data and the SPI packet
  accel_txdata[0] = 
    (unsigned char)((address & ACCEL_SPI_ADDR) | 
                     ACCEL_SPI_WRITE | ACCEL_SPI_INC);

  accel_spi_packet.finished = 0;
  accel_spi_packet.num_words = count+1;
  
  spi_send_packet (&accel_spi_packet);
}

unsigned char accel_read_reg_block(unsigned int address)
{
  // Wait for the spi_packet to be free
  while (accel_spi_packet.finished == 0) ;
  // Write data
  accel_read_reg (address);
  // Wait again
  while (accel_spi_packet.finished == 0) ;

  return accel_rxdata[1];
}

void accel_read_reg (unsigned int address)
{
  if (accel_spi_packet.finished == 0)
  {
    error_set(1 << ID_ACCEL);
    return;
  }

  // Prepare the address field and the SPI packet
  accel_txdata[0] = 
    (unsigned char)((address & ACCEL_SPI_ADDR) | ACCEL_SPI_READ);

  accel_spi_packet.finished = 0;
  // We send two words, even though the second one is a dummy. This is
  //  so that we send the extra clock cycles to receive the data.
  accel_spi_packet.num_words = 2;
  
  spi_send_packet (&accel_spi_packet);
}

/** Reads multiple consecutive registers starting at the given address.
  * This routine does not block. Rather, the user should call
  * accel_data_ready() to determine whether the requested data has been
  * received.
  */
void accel_read (unsigned int address, unsigned count)
{
  if (accel_spi_packet.finished == 0 || count >= ACCEL_BUFFER_SIZE)
  {
    error_set(1 << ID_ACCEL);
    return;
  }

  // Prepare the data and the SPI packet
  accel_txdata[0] = 
    (unsigned char)((address & ACCEL_SPI_ADDR) | 
                     ACCEL_SPI_READ | ACCEL_SPI_INC);

  accel_spi_packet.finished = 0;
  accel_spi_packet.num_words = count+1;
  
  spi_send_packet (&accel_spi_packet);
}
