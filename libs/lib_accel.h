/*
 *  lib_accel.h
 *  
 *
 *  Created by Marc G. Bellemare on 21/07/08.
 *
 *  Accelerometer driver.
 *  Minimum startup time before data becomes valid: at least 50ms.
 */

#ifndef LIB_ACCEL_H
#define LIB_ACCEL_H

#include "lib_spi.h"

// Maximum number of registers that can be read/written at once. The 
//  actual number is one less than the buffer size, as we need to write
//  the address and flags in the first byte.
#define ACCEL_BUFFER_SIZE 8

// How many axes we are reporting (no more than 3!)
#define ACCEL_NUM_AXES    3

/** Register definitions for the accelerometer. 
  * Use these when an address is required. 
  */
#define ACCEL_REG_WHOAMI    0x0F
#define ACCEL_REG_CALIB     ACCEL_REG_OFFSET_X
#define ACCEL_REG_OFFSET_X  0x16
#define ACCEL_REG_OFFSET_Y  0x17
#define ACCEL_REG_OFFSET_Z  0x18
#define ACCEL_REG_GAIN_X    0x19
#define ACCEL_REG_GAIN_Y    0x1A
#define ACCEL_REG_GAIN_Z    0x1B

#define ACCEL_REG_CTRL1     0x20
#define ACCEL_REG_CTRL2     0x21
#define ACCEL_REG_CTRL3     0x22

#define ACCEL_REG_OUT       ACCEL_REG_STATUS
#define ACCEL_REG_STATUS    0x27
#define ACCEL_REG_OUTX_L    0x28
#define ACCEL_REG_OUTX_H    0x29
#define ACCEL_REG_OUTY_L    0x2A
#define ACCEL_REG_OUTY_H    0x2B
#define ACCEL_REG_OUTZ_L    0x2C
#define ACCEL_REG_OUTZ_H    0x2D

// The SPI device ID for the accelerometer
#define ACCEL_SPI_DEVICE_ID 0

/** Some flags used for configuring the accelerometer. The SPI sends data
  * in big-endian, but initially the accel is in little-endian.
  */
//#define ACCEL_CTRL1_POWER_ON          0xD0
//#define ACCEL_CTRL2_BIGENDIAN_LITTLE  0x04

/** CTRL1 is: PD1-0 DF1-0 ST Zen Yen Xen
  * Corresponding to 11010111
  * Power up, 160Hz, self-test disabled, enable all axes
  * This is in big-endian
  */
#define ACCEL_CTRL1_SETTINGS          0xD7

/** CTRL2 is: FS BDU BLE BOOT IEN DRDY SIM DAS
  * We set FS (full-scale=1), block data update (1), big-endian (1),
  *  interrupt disabled (IEN=0), data-ready generation enabled (DRDY=1)
  * SIM should be 0 (4-wire SPI interface)
  * I have no clue about DAS, data alignment selection
  */
#define ACCEL_CTRL2_FULLSCALE (1 << 7)
#define ACCEL_CTRL2_BDU       (1 << 6)
#define ACCEL_CTRL2_BLE       (1 << 5)
#define ACCEL_CTRL2_IEN       (1 << 3)
#define ACCEL_CTRL2_DRDY      (1 << 2)

// Don't remove BLE or I will come and get you!
#define ACCEL_CTRL2_SETTINGS          (ACCEL_CTRL2_BDU | \
                                       ACCEL_CTRL2_DRDY)

#define ACCEL_WHOAMI_VALUE    0x3A

/** Some macros defining the protocol to communicate with the accelerometer
  * via the SPI. These values are in big-endian, as the SPI runs in big-endian.
  * bits 0-5 are for the register address; bit 6 is to select incrementing
  * of the address (multiple reads/writes) and bit 7 selects read/write.
  */
#define ACCEL_SPI_ADDR        0x3F
#define ACCEL_SPI_WRITE       0x00
#define ACCEL_SPI_READ        (1 << 7)
#define ACCEL_SPI_INC         (1 << 6)

/**
  * Initialization routine for the accelerometer driver.
  */
void accel_init();

/**
  * Event function called by the 100Hz timer.
  */
void accel_event();

/** Writes to a single register, specified by the address. 
  * THIS ROUTINE BLOCKS
  */
void accel_write_reg_block (unsigned int address, unsigned int data);

/** Reads a single register, specified by the address. 
  * THIS ROUTINE BLOCKS
  */
unsigned char accel_read_reg_block (unsigned int address);

/** Writes to a single register, specified by the address. */
void accel_write_reg (unsigned int address, unsigned int data);

/** Writes to 'count' consecutive registers starting at the given address.
  * The data to be written should be placed in accel_txdata[1]...
  *  accel_txdata[count].
  */
void accel_write (unsigned int address, unsigned int count);

/** Reads a single register at the given address.
  *
  * The received data is available in rxdata[1] and is also returned.
  */
void accel_read_reg (unsigned int address);

/** Reads multiple consecutive registers starting at the given address.
  * This routine does not block. Rather, the user should call
  * accel_data_ready() to determine whether the requested data has been
  * received.
  *
  * The received data will be in accel_rxdata[1].
  */
void accel_read (unsigned int address, unsigned count);

// Some variables needed by our inline functions
extern struct spi_packet accel_spi_packet;
extern unsigned int accel_status;
extern short accel_output[ACCEL_NUM_AXES];


/** Returns whether new data is ready.
  * Requesting a new read before the previous one is complete is not 
  * properly handled.
  *
  * The received data is available in rxdata[1]...rxdata[ACCEL_BUFFER_SIZE].
  */
__inline unsigned int accel_ready()
{
  return (accel_spi_packet.finished == 1);
}

/** Returns the accelerometer-provided status. */
__inline unsigned char accel_get_status()
{
  return accel_status;
}

/** Returns the current output of the accelerometer for a particular axis.
  * Axes 0,1,2 corresponds to X, Y and Z, respectively.
  */
__inline short accel_get_output(int axis)
{
  return accel_output[axis];
}

#endif /* LIB_ACCEL_H */

