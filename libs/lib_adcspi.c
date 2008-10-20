/*
 *  lib_adcspi.c
 *  
 *
 *  Created by Marc G. Bellemare on 1/08/08.
 *
 */

#include "armio.h"
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

unsigned int adcspi_command = 0;
unsigned int adcspi_output[ADCSPI_NUM_DEVICES][ADCSPI_OUTPUTS_PER_DEVICE];

unsigned int adcspi_init_data = 0xFFFF;
unsigned int adcspi_start_data = 0xFFB0;
unsigned int adcspi_dummy;

struct spi_packet adcspi_command_packet[ADCSPI_NUM_DEVICES][ADCSPI_OUTPUTS_PER_DEVICE];
struct spi_packet adcspi_init_packet[ADCSPI_NUM_DEVICES];
struct spi_packet adcspi_start_packet[ADCSPI_NUM_DEVICES];

int adcspi_init()
{
  int i, j;
  struct spi_packet * packet;
  
  for(i = 0; i < ADCSPI_NUM_DEVICES; i++) { 
    packet = &adcspi_init_packet[i];

    packet->device_id = i + ADCSPI_DEVICE_ID_BASE;
    packet->num_words = 1;
    packet->data_to_write = &adcspi_init_data;
    packet->read_data = &adcspi_dummy;
    packet->finished = 0;

    packet = &adcspi_start_packet[i];

    packet->device_id = i + ADCSPI_DEVICE_ID_BASE;
    packet->num_words = 1;
    packet->data_to_write = &adcspi_start_data;
    packet->read_data = &adcspi_dummy;
    packet->finished = 0;

    for (j = 0; j < ADCSPI_OUTPUTS_PER_DEVICE; j++)
    {
        packet = &adcspi_command_packet[i][j];

        packet->device_id = i + ADCSPI_DEVICE_ID_BASE;
        packet->num_words = 1;
        packet->data_to_write = &adcspi_command;
        packet->read_data = &adcspi_output[i][j];
        packet->finished = 0;
    }
    spi_send_packet(&adcspi_init_packet[i]);
  }
  
  return 0;
}

int adcspi_event()
{
  int i;

  for(i = 0; i < ADCSPI_NUM_DEVICES; i++)
    adcspi_read_data(i);

  return 0;
}

/** Sends packets to read in data from the ADC (only for selected inputs) */
void adcspi_read_data(unsigned int device)
{
  int i;

  spi_send_packet(&adcspi_start_packet[device]);
  for(i = 0; i < ADCSPI_OUTPUTS_PER_DEVICE; i++) {
    spi_send_packet(&adcspi_command_packet[device][i]);
  }
  
}

int adcspi_get_output(unsigned int device, unsigned int index)
{
  if(device >= ADCSPI_NUM_DEVICES || index >= ADCSPI_OUTPUTS_PER_DEVICE)
    return -1;
  // Return the output corr. to the index (only the 12 lower bits are data)
  // Note that the data might correspond to the previous time step's,
  //  e.g. if the new output has not been read yet
  // In particular, the result will be undefined if 'index' is not selected
  //  or if this is the first event cycle after selecting 'index'
  // @@@ handle this - but can't simply check for finished == 0
  // shift by 1 for issues reading first bit, by 2 for noise
  return (adcspi_output[device][index] & 0x1FFF) >> 3;
}

