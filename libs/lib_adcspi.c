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
// We need 64 of these because of the way the spi_packet works :(
unsigned int adcspi_dummy_tx[ADCSPI_NUM_OUTPUTS];
// Dummy variable that receives unused data when transmitting (e.g. the control
//  register)
unsigned int adcspi_dummy_rx;

// Buffer containing the actual output values from the ADC
unsigned int adcspi_output[ADCSPI_NUM_OUTPUTS];

// Data to be written to the shadow register
// Bit x being set causes input 15-x to be converted
unsigned int adcspi_shadow_reg[ADCSPI_NUM_DEVICES];

// Packet to send to the control register
unsigned int adcspi_control_reg[ADCSPI_NUM_DEVICES];

// We use one packet per device. The disadvantage of doing this is that we
//  must request all data (16 bytes). Requesting variable length data (by
//  setting num_words < 16 and selecting outputs with the shadow register)
//  would require shifting data around after receiving it using the SPI.
struct spi_packet adcspi_read_packets[ADCSPI_NUM_DEVICES];
struct spi_packet adcspi_control_packet[ADCSPI_NUM_DEVICES];
struct spi_packet adcspi_shadow_packet[ADCSPI_NUM_DEVICES];

// This flag tells us whether new inputs were selected since the last event
int adcspi_new_selection[ADCSPI_NUM_DEVICES];

int adcspi_init()
{
  int i;
  struct spi_packet * packet;

  // Set up the read registers
  
  for (i = 0; i < ADCSPI_NUM_DEVICES; i++)
  {
    // Set up the dummy packet we send to read data
    adcspi_dummy_tx[i] = ADCSPI_READ;
    
    packet = &adcspi_read_packets[i];

    packet->device_id = i + ADCSPI_DEVICE_ID_BASE;
    packet->num_words = ADCSPI_OUTPUTS_PER_DEVICE;
    packet->data_to_write = &adcspi_dummy_tx[i * ADCSPI_OUTPUTS_PER_DEVICE];
    packet->read_data = &adcspi_output[i * ADCSPI_OUTPUTS_PER_DEVICE];
    packet->finished = 1;
  
    // Set up each off-board ADC's packets and power-up the chip itself
    packet = &adcspi_control_packet[i];

    packet->device_id = i + ADCSPI_DEVICE_ID_BASE;
    packet->num_words = 1;
    packet->data_to_write = &adcspi_control_reg[i];
    packet->read_data = &adcspi_dummy_rx;
    // Redundant line (we set finished a few lines down), but leave it here 
    //  for clarity and future code changes
    packet->finished = 1;

    packet = &adcspi_shadow_packet[i];

    packet->device_id = i + ADCSPI_DEVICE_ID_BASE;
    packet->num_words = 1;
    packet->data_to_write = &adcspi_shadow_reg[i];
    packet->read_data = &adcspi_dummy_rx;
    packet->finished = 1;
  
    adcspi_new_selection[i] = 0;

    // Power up the chip! We do so by sending all ones (the line should be
    //  tied high, but this is harder to achieve)
    adcspi_control_reg[i] = ADCSPI_CTRLREG_POWER_UP;
  
    adcspi_control_packet[i].finished = 0;
    // @@@ replace by spi_send_packet_block?
    spi_send_packet(&adcspi_control_packet[i]);
    while (!adcspi_control_packet[i].finished)
      ;
  
    // Send the initial input selection
    // This MUST be 0xFFFF using the current code
    adcspi_shadow_reg[i] = ADCSPI_DEFAULT_INPUTS;
    adcspi_send_select(i);
  }

  return 0;
}

int adcspi_event()
{
  int i;
  // Send the new input selection, if there is one
  for (i = 0; i < ADCSPI_NUM_DEVICES; i++)
  {
    // Select regardless of new selection, for sync'ing purposes
    adcspi_send_select(i);
    adcspi_new_selection[i] = 0;
  }
      // Test for error in addresses (only if no new data was selected)
    // adcspi_test_addresses();

  // Request new data in
  for (i = 0; i < ADCSPI_NUM_DEVICES; i++)
    adcspi_read_data(i);
  return 0;
}


void adcspi_select(int input)
{
  // Note - these operations are fast because ADCSPI_OUTPUTS_PER_DEVICE is
  //  a multiple of 2. If time, check out whether the compiler is smart and
  //  uses right shift and bitwise AND.
  int device = input / ADCSPI_OUTPUTS_PER_DEVICE;
  input = input % ADCSPI_OUTPUTS_PER_DEVICE;

  // Sets the corresponding bit (flip first)
  input = ADCSPI_OUTPUTS_PER_DEVICE - 1 - input;
  adcspi_shadow_reg[device] |= (1 << input) & 0xFFFF;
  // Flag the driver as needing to re-send the shadow register
  adcspi_new_selection[device] = 1;
}

void adcspi_deselect(int input)
{
  // See comment in adcspi_select
  int device = input / ADCSPI_OUTPUTS_PER_DEVICE;
  input = input % ADCSPI_OUTPUTS_PER_DEVICE;
  
  // Clear the corresponding bit
  input = ADCSPI_OUTPUTS_PER_DEVICE - 1 - input;
  adcspi_shadow_reg[device] &= ~((1 << input) & 0xFFFF);
  adcspi_new_selection[device] = 1;
}

int adcspi_is_selected(int input)
{
  // See comment in adcspi_select
  int device = input / ADCSPI_OUTPUTS_PER_DEVICE;
  input = input % ADCSPI_OUTPUTS_PER_DEVICE;
  
  input = ADCSPI_OUTPUTS_PER_DEVICE - 1 - input;
  return (adcspi_shadow_reg[device] & (1 << input));
}

void adcspi_send_select(int device)
{
  // Send the control packet, then the shadow packet
  adcspi_control_reg[device] = (ADCSPI_CONTROL_FLAGS | ADCSPI_WRITE | 
    ADCSPI_SELECT_SHADOW) << 4;

  if(!adcspi_control_packet[device].finished) {
    error_set(ERR_ADC_SPI);
    return;
  }
  adcspi_control_packet[device].finished = 0;
  spi_send_packet(&adcspi_control_packet[device]);
  if(!adcspi_shadow_packet[device].finished) {
    error_set(ERR_ADC_SPI);
    return;
  } 
  adcspi_shadow_packet[device].finished = 0;
  // Shadow packet contains bit-mapped inputs to be converted
  spi_send_packet(&adcspi_shadow_packet[device]);
}

/** Sends packets to read in data from the ADC (only for selected inputs) */
void adcspi_read_data(int device)
{
  if(!adcspi_read_packets[device].finished) {
    error_set(ERR_ADC_SPI);
    return;
  }
  adcspi_read_packets[device].finished = 0;

  // We request all data (16 packets)
  spi_send_packet(&adcspi_read_packets[device]);
}

int adcspi_get_output(int index)
{
  // Return the output corr. to the index (only the 12 lower bits are data)
  // Note that the data might correspond to the previous time step's,
  //  e.g. if the new output has not been read yet
  // In particular, the result will be undefined if 'index' is not selected
  //  or if this is the first event cycle after selecting 'index'
  // @@@ handle this - but can't simply check for finished == 0
  // shift by 1 for issues reading first bit, by 2 for noise
  return (adcspi_output[index] & 0x1FFF) >> 3;
}

int adcspi_get_address(int index)
{
  // Returns the address corresponding to the index; should be == index % 16.
  return (adcspi_output[index] & 0xF000) >> 12;
}

void adcspi_test_addresses(int device)
{
  int bitmap = adcspi_shadow_reg[device];
  int index = 0;
  int address;

  // Go through the received data and test each address (note: we start
  //  with the first output, which corresponds to the last bit of the shadow
  //  register)
  while (index < ADCSPI_OUTPUTS_PER_DEVICE)
  {
    // @@@ test if packet was not sent but bit is set? 
    if (bitmap & 0x8000)
    {
      address = (adcspi_output[device*ADCSPI_OUTPUTS_PER_DEVICE+index] & 
        0xF000) >> 12;
      if (address != index)
        error_set(ERR_ADC_SPI);
    }
    bitmap = bitmap << 1;
    index++;
  }
}
