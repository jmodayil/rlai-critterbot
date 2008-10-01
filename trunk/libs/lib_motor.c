
#include "lib_motor.h"

event_s motor_event_s = {
  motor_init,
  motor_event,
  0
};

unsigned int motor_tx_data[MOTOR_NUM_MOTORS][MOTOR_NUM_BYTES];
unsigned int motor_rx_data[MOTOR_NUM_MOTORS][MOTOR_NUM_BYTES];
unsigned int power_tx_data;
unsigned int power_rx_data;

struct spi_packet motor_packet[MOTOR_NUM_MOTORS]; 
struct spi_packet power_packet;

volatile int motor_test;

char motor_speed[MOTOR_NUM_MOTORS];

static char motor_slew_interval[MOTOR_NUM_MOTORS];
static char motor_slew_each[MOTOR_NUM_MOTORS];
static char motor_slew_mod;
static char motor_slew_count;

int motor_init() {

  int i;

  for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
    motor_packet[i].device_id = 9 + i;
    motor_packet[i].num_words = MOTOR_NUM_BYTES;
    motor_packet[i].data_to_write = &motor_tx_data[i][0];
    motor_packet[i].read_data = &motor_rx_data[i][0];
    motor_packet[i].finished = 1;
    
    motor_init_packet(i);
  }

  power_packet.device_id = 8;
  power_packet.num_words = 1;
  power_packet.data_to_write = &power_tx_data;
  power_packet.read_data = &power_rx_data;
  power_packet.finished = 1;

  return 0;  
}

int motor_event() {
  
  unsigned int volt;
  int i;

  if(motor_slew_count < MOTOR_SLEW_TIME) {
    for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
      motor_speed[i] += motor_slew_each[i];
      if(motor_slew_mod == motor_slew_interval[i]) {
        motor_slew_mod = 0;
        motor_speed[i] += (motor_slew_each[i] > 0) ? 1 : -1;
      }
      motor_slew_mod++;
      motor_slew_count++;
    }
  }


  volt = motor_get_voltage();

  for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
    motor_tx_data[i][1] = motor_speed[i];
    motor_tx_data[i][2] = volt;  
    spi_send_packet(&motor_packet[i]);
  }
  spi_send_packet(&power_packet); 
  for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
    if((motor_rx_data[i][4] & 0xFF) != MOTOR_PACKET_HEADER)
      armprintf("!Motor %d: %d %d %d %d\r", i, 
        motor_rx_data[i][1] & 0xFF, 
        motor_rx_data[i][2] & 0xFF, 
        motor_rx_data[i][3] & 0xFF,
        motor_rx_data[i][4] & 0xFF); 
  }
  if(motor_event_s.event_count % 100 == 0) {
    for(i = 0; i < MOTOR_NUM_MOTORS; i++)
      armprintf("Motor %d: %d %d %d %d\r", i, 
        motor_rx_data[i][1] & 0xFF, 
        motor_rx_data[i][2] & 0xFF, 
        motor_rx_data[i][3] & 0xFF,
        motor_rx_data[i][4] & 0xFF); 
    armprintf("\rPower: %d\r", power_rx_data & 0xFF);
    motor_test++;
  }
  return 0; 
}

void motor_set_speed(int motor, int speed) {

  char temp;

  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return;

  if(speed < -MOTOR_MAX_SPEED)
    speed = -MOTOR_MAX_SPEED;
  if(speed > MOTOR_MAX_SPEED) 
    speed = MOTOR_MAX_SPEED;

  motor_slew_mod = 1;
  motor_slew_count = 0;
  motor_slew_each[motor] = (speed - motor_speed[motor]) / MOTOR_SLEW_TIME;
  motor_slew_interval[motor] = (speed - motor_speed[motor]) % MOTOR_SLEW_TIME;
  if(motor_slew_interval[motor] != 0)
    motor_slew_interval[motor] = MOTOR_SLEW_TIME / motor_slew_interval[motor];
  //motor_speed[motor] = speed & 0xFF;
}

unsigned char motor_get_voltage() {
  
  unsigned int temp;
 
	if(0 == (power_rx_data & 0xFF))
		return 255;
  temp = power_rx_data & 0xFF;
  return ((temp + 148) * 7) / 27;
}

void motor_init_packet(int motor) {

  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return;

  motor_tx_data[motor][0] = MOTOR_PACKET_HEADER;
  motor_tx_data[motor][1] = 0;
  motor_tx_data[motor][2] = 255;

}

void motor_set_pwm(int motor, int pwm) {

  unsigned int temp;

  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return;

  if(pwm < -MOTOR_MAX_PWM || pwm > MOTOR_MAX_PWM)
    return;

  temp = power_rx_data & 0xFF;
  temp = ((temp + 148) * 7) / 27; 
  motor_tx_data[motor][0] = MOTOR_PWM_HEADER;
  motor_tx_data[motor][1] = pwm & 0xFF;
  motor_tx_data[motor][2] = temp & 0xFF;
}

char motor_clicks(int motor) {

  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return 0;

  return motor_rx_data[motor][0] & 0xFF;
}

unsigned char motor_current(int motor) {

  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return 0;

  return motor_rx_data[motor][1] & 0xFF;
}

unsigned char motor_temp(int motor) {
  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return 0;

  return motor_rx_data[motor][2] & 0xFF;
}

