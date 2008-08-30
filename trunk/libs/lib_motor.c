
#include "lib_motor.h"

event_s motor_event_s = {
  motor_init,
  motor_event,
  0
};

unsigned int motor_tx_data[MOTOR_NUM_MOTORS][MOTOR_NUM_BYTES];
unsigned int motor_rx_data[MOTOR_NUM_MOTORS][MOTOR_NUM_BYTES];

struct spi_packet motor_packet[MOTOR_NUM_MOTORS]; 

volatile int motor_test;

int motor_init() {

  int i;

  for(i = 0; i < MOTOR_NUM_BYTES; i++) {
    motor_packet[i].device_id = 9 + i;
    motor_packet[i].num_words = MOTOR_NUM_BYTES;
    motor_packet[i].data_to_write = &motor_tx_data[i][0];
    motor_packet[i].read_data = &motor_rx_data[i][0];
    motor_packet[i].finished = 1;
  }
  return 0;  
}

int motor_event() {
  
  int i;
  
  for(i = 0; i < MOTOR_NUM_MOTORS; i++)
    spi_send_packet(&motor_packet[i]);
  
  if(motor_event_s.event_count % 100 == 0) {
    for(i = 0; i < 1; i++)
      armprintf("Motor %d: %d %d %d\r", i, motor_rx_data[i][0] & 0xFF,
         motor_rx_data[i][1] & 0xFF, motor_rx_data[i][2] & 0xFF); 
    armprintf("\r");
    }
  return 0; 
}

void motor_set_speed(int motor, int speed) {

  if(motor < 0 || motor > MOTOR_NUM_MOTORS)
    return;

  if(speed < -MOTOR_MAX_SPEED || speed > MOTOR_MAX_SPEED)
    return;

  motor_tx_data[motor][0] = speed & 0xFF;
}
