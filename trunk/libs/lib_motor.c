
#include "lib_motor.h"

event_s motor_event_s = {
  motor_init,
  motor_event,
  0
};

unsigned int motor_tx_data;
unsigned int motor_rx_data;

struct spi_packet motor_packet; 

int motor_init() {

  motor_tx_data = 0;
  
  motor_packet.device_id = 9;
  motor_packet.num_words = 1;
  motor_packet.data_to_write = &motor_tx_data;
  motor_packet.read_data = &motor_rx_data;
  motor_packet.finished = 1;
  return 0;  
}

int motor_event() {
  
  spi_send_packet(&motor_packet);

  if(motor_event_s.event_count % 100 == 0)
    armprintf("Motor packet = %u\r", (unsigned char)motor_rx_data);
  
  return 0; 
}

void motor_set_speed(int motor, int speed) {

  if(motor < 0 || motor > MOTOR_NUM_MOTORS)
    return;

  if(speed < -MOTOR_MAX_SPEED || speed > MOTOR_MAX_SPEED)
    return;

  motor_tx_data = speed & 0xFF;
}
