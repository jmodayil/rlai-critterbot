/*
 *  lib_ui_test.c
 *  
 *
 *  Created by Marc G. Bellemare on 7/07/08.
 *  Last Revision: 7/07/08
 *
 *
 *  Code to test lib_ui.c, the user interface.
 *
 */

#include <stdio.h>
#include "compiler.h"
#include "armconfig.h"

#include "armio.h"
#include "lib_ledctl.h"
#include "lib_ssc.h"
#include "lib_ui.h"

#include "lib_spi.h"
#include "lib_accel.h"

int main()
{
  struct spi_packet testdata;
  unsigned int send, receive;
  
  testdata.device_id = 8;
  testdata.num_words = 1;
  testdata.data_to_write = &send;
  testdata.read_data = &receive;
  testdata.finished = 0;
  
  send = 0;
  receive = 0;
  // Initialize the serial port and the LED controller
  init_serial_port_stdio();
  ssc_init();
  armprintf("Initialized ssc.\n"); 
  spi_init();
  armprintf("Initialized spi.\n");
  ledctl_init();
  armprintf("Initialized ledctl.\n");
  // @@@ This function will have to be replaced when we get a proper 100Hz
  //  event clock
  ledctl_inittimer();
  armprintf("Initialized timer.\n");
  accel_init();
  armprintf ("Initialized accelerometer.\n");

  // Repeatedly loop for events
  while (1)
  {
    ui_event();
    //spi_send_packet(&testdata);
    //while(!testdata.finished);
    //testdata.finished = 0;
    //armprintf("spi data back: %d\n", (receive & 0xFF));
    //send = receive;
    //for(i=0;i<1000000;i++);
  }
}
