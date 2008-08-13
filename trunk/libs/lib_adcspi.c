/*
 *  lib_adcspi.c
 *  
 *
 *  Created by Marc G. Bellemare on 1/08/08.
 *
 */

#include "compiler.h"
#include "armconfig.h"
#include "lib_spi.h"
#include "lib_error.h"
#include "lib_adcspi.h"

event_s adcspi_event_s = {
  adcspi_init,
  adcspi_event,
  0
};

// Dummy data that we send to read in data; bit 15 must be 0 (READ)
unsigned int adcspi_dummy_tx;
unsigned int adcspi_dummy_rx;

// Buffer containing the actual output values from the ADC
unsigned int adcspi_output[ADCSPI_NUM_OUTPUTS];

// Data to be written to the shadow register
// Bit x being set causes input 15-x to be converted
unsigned int adcspi_shadow_reg;

// Packet to send to the control register
unsigned int adcspi_control_reg;

// There are two ways of doing this; either by requesting a variable-sized
//  packet or by sending a number of packets. The former requires us to shift
//  the data after reading it, as well as have a 16 ints buffer of dummy 
//  data to send. Hence I favor the 16 SPI packets, despite the slight 
//  additional memory overhead.
struct spi_packet adcspi_read_packets[ADCSPI_NUM_OUTPUTS];
struct spi_packet adcspi_control_packet;
struct spi_packet adcspi_shadow_packet;

// This flag tells us whether new inputs were selected since the last event
int adcspi_new_selection;

int adcspi_init()
{
  int i;
 
  // Set up the dummy packet we send to read data
  adcspi_dummy_tx = ADCSPI_READ;
  adcspi_new_selection = 0;
  adcspi_shadow_reg = 0;

  // Set up the read registers
  
  for (i = 0; i < ADCSPI_NUM_OUTPUTS; i++)
  {
    adcspi_read_packets[i].device_id = ADCSPI_DEVICE_ID;
    adcspi_read_packets[i].num_words = 1;
    adcspi_read_packets[i].data_to_write = &adcspi_dummy_tx;
    adcspi_read_packets[i].read_data = &adcspi_output[i];
    adcspi_read_packets[i].finished = 0;
  }

  // Set up the control registers
  adcspi_control_packet.device_id = ADCSPI_DEVICE_ID;
  adcspi_control_packet.num_words = 1;
  adcspi_control_packet.data_to_write = &adcspi_control_reg;
  adcspi_control_packet.read_data = &adcspi_dummy_rx;
  adcspi_control_packet.finished = 0;

  adcspi_shadow_packet.device_id = ADCSPI_DEVICE_ID;
  adcspi_shadow_packet.num_words = 1;
  adcspi_shadow_packet.data_to_write = &adcspi_shadow_reg;
  adcspi_shadow_packet.read_data = &adcspi_dummy_rx;
  adcspi_shadow_packet.finished = 1;

  // Power up the chip! We do so by sending all ones (the line should be
  //  tied high, but this is harder to achieve)
  adcspi_control_reg = 0xFFFF;
  adcspi_shadow_reg = 0xFFFC;
  
  adcspi_control_packet.finished = 0;
  // @@@ replace by spi_send_packet_block?
  spi_send_packet(&adcspi_control_packet);
  while (!adcspi_control_packet.finished)
    ;

  // Send the initial input selection
  adcspi_send_select();
  return 0;
}

int adcspi_event()
{
  // Send the new input selection, if there is one
  if (adcspi_new_selection)
  {
    adcspi_send_select();
    adcspi_new_selection = 0;
  }
  else
    // Test for error in addresses (only if no new data was selected)
    adcspi_test_addresses();


  // Request new data in
  adcspi_read_data();
  return 0;
}


void adcspi_select(int input)
{
  // Sets the corresponding bit (flip first)
  input = ADCSPI_NUM_OUTPUTS - 1 - input;
  adcspi_shadow_reg |= (1 << input) & 0xFFFF;
  // Flag the driver as needing to re-send the shadow register
  adcspi_new_selection = 1;
}

void adcspi_deselect(int input)
{
  // Clear the corresponding bit
  input = ADCSPI_NUM_OUTPUTS - 1 - input;
  adcspi_shadow_reg &= ~((1 << input) & 0xFFFF);
  adcspi_new_selection = 1;
}

int adcspi_is_selected(int input)
{
  input = ADCSPI_NUM_OUTPUTS - 1 - input;
  return (adcspi_shadow_reg & (1 << input));
}

void adcspi_send_select()
{
  // Send the control packet, then the shadow packet
  adcspi_control_reg = (ADCSPI_CONTROL_FLAGS | ADCSPI_WRITE | 
    ADCSPI_SELECT_SHADOW) << 4;

  spi_send_packet(&adcspi_control_packet);
  // Shadow packet contains bit-mapped inputs to be converted
  spi_send_packet(&adcspi_shadow_packet);
}

/** Sends packets to read in data from the ADC (only for selected inputs) */
void adcspi_read_data()
{
  int bitmap = adcspi_shadow_reg;
  // Bit 15 corresponds to input 0 and vice-versa
  int n = ADCSPI_NUM_OUTPUTS-1;
  int index;

  while (n >= 0)
  {
    index = ADCSPI_NUM_OUTPUTS-1-n;
    adcspi_read_packets[index].finished = 0;
    // If the MSB is set, read the corr. packet
    if (bitmap & 0x8000) 
      spi_send_packet(&adcspi_read_packets[index]);
    bitmap = bitmap << 1;
    n--;
  }
}

int adcspi_get_output(int index)
{
  // Return the output corr. to the index (only the 12 lower bits are data)
  // Note that the data might correspond to the previous time step's,
  //  e.g. if the new output has not been read yet
  // In particular, the result will be undefined if 'index' is not selected
  //  or if this is the first event cycle after selecting 'index'
  // @@@ handle this - but can't simply check for finished == 0
  return (adcspi_output[index] & 0x1FFF) >> 1;
}

void adcspi_test_addresses()
{
  int bitmap = adcspi_shadow_reg;
  int n = ADCSPI_NUM_OUTPUTS-1;
  int index;
  int address;

  // Go through the received data and test each address
  while (n >= 0)
  {
    index = ADCSPI_NUM_OUTPUTS-1-n;

    // @@@ test if packet was not sent but bit is set? 
    if (bitmap & 0x8000)
    {
      address = (adcspi_output[index] & 0xF000) >> 12;
      if (address != index)
        error_set(ERR_ADC_SPI);
    }
    bitmap = bitmap << 1;
    n--;
  }
}
