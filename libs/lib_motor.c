
#include "lib_motor.h"
#include "lib_mi.h"

event_s motor_event_s = {
  motor_init,
  motor_event,
  0
};

unsigned int motor_tx_data[MOTOR_NUM_MOTORS][MOTOR_NUM_BYTES];
unsigned int motor_rx_data[MOTOR_NUM_MOTORS][MOTOR_NUM_BYTES];
unsigned int power_tx_data[MOTOR_PWR_BYTES];
unsigned int power_rx_data[MOTOR_PWR_BYTES];

struct spi_packet motor_packet[MOTOR_NUM_MOTORS]; 
struct spi_packet power_packet;

signed char motor_speed[MOTOR_NUM_MOTORS];
signed char motor_speed_final[MOTOR_NUM_MOTORS];

static char motor_slew_steps;
static float motor_speed_float[MOTOR_NUM_MOTORS];
static float motor_slew_interval[MOTOR_NUM_MOTORS];
static char motor_slew_count;

static unsigned int motor_timeout_count;

int motor_init() {

  int i;

  motor_slew_steps = 0;
  motor_slew_count = 1;

  for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
    motor_packet[i].device_id = 9 + i;
    motor_packet[i].num_words = MOTOR_NUM_BYTES;
    motor_packet[i].data_to_write = &motor_tx_data[i][0];
    motor_packet[i].read_data = &motor_rx_data[i][0];
    motor_packet[i].finished = 1;
    
    motor_init_packet(i);
  }

  power_packet.device_id = 8;
  power_packet.num_words = MOTOR_PWR_BYTES;
  power_packet.data_to_write = &power_tx_data[0];
  power_packet.read_data = &power_rx_data[0];
  power_packet.finished = 1;

  return 0;  
}

int motor_event() {
  
  unsigned int volt;
  int i;

  if(++motor_timeout_count == MOTOR_TIMEOUT) {
    motor_set_speed_slew(0,0,0);
  }

  if(motor_slew_count < motor_slew_steps) {
    for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
      motor_speed_float[i] += motor_slew_interval[i];
      motor_speed[i] = (signed char) motor_speed_float[i];
    }
    motor_slew_count++;  
  }
  else if(motor_slew_count == motor_slew_steps) {
    for(i = 0; i < MOTOR_NUM_MOTORS; i++)
      motor_speed[i] = (signed char)  motor_speed_final[i];
    motor_slew_count++;
  }

  
  volt = motor_get_voltage();
  
  spi_send_packet(&power_packet); 
  for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
    motor_tx_data[i][1] = (unsigned char) motor_speed[i];
    motor_tx_data[i][2] = volt;  
    spi_send_packet(&motor_packet[i]);
  }
  for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
    if((motor_rx_data[i][4] & 0xFF) != MOTOR_SPI_PADDING)
      error_set(ERR_MOTOR_ALIGN);
  }
      /*armprintf("!Motor %d: %d %d %d %d\r", i, 
        motor_rx_data[i][1] & 0xFF, 
        motor_rx_data[i][2] & 0xFF, 
  
        motor_rx_data[i][4] & 0xFF); 
  }
    for(i = 0; i < MOTOR_NUM_MOTORS; i++)
      armprintf("Motor %d: %d %d %d %d %d\r", i, 
        motor_rx_data[i][0] & 0xFF,
        motor_rx_data[i][1] & 0xFF, 
        motor_rx_data[i][2] & 0xFF, 
        motor_rx_data[i][3] & 0xFF,
        motor_rx_data[i][4] & 0xFF); 
    //armprintf("\rPower: %d\r", power_rx_data & 0xFF);
  */return 0; 
}

void motor_set_speed(int motor, signed char speed) {

  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return;

  if(speed < -MOTOR_MAX_SPEED)
    speed = -MOTOR_MAX_SPEED;
  if(speed > MOTOR_MAX_SPEED) 
    speed = MOTOR_MAX_SPEED;

  //if(motor_speed_final[motor] == speed)
  //  return;

  /*motor_slew_count = 0;
  motor_speed_float[motor] = motor_speed[motor];
  motor_speed_final[motor] = speed;
  motor_slew_interval[motor] = (float)(speed - motor_speed[motor]) / 
                                (float)MOTOR_SLEW_TIME;*/
  motor_speed[motor] = speed;
}

void motor_set_speed_xytheta(signed char xvel, signed char yvel, 
    signed char tvel) {
 
  int max;
  int m100, m220, m340;
  
  m100 = (xvel * XSC100 + yvel * YSC100 + tvel * TSC100) / 1024;
  m220 = (xvel * XSC220 + yvel * YSC220 + tvel * TSC220) / 1024;
  m340 = (xvel * XSC340 + yvel * YSC340 + tvel * TSC340) / 1024;  

  max = ABS(m100);
  if(ABS(m220) > max)
    max = ABS(m220);
  if(ABS(m340) > max)
    max = ABS(m340);
  if(max > MOTOR_MAX_SPEED) {
    m100 = (m100 * MOTOR_MAX_SPEED) / max;
    m220 = (m220 * MOTOR_MAX_SPEED) / max;
    m340 = (m340 * MOTOR_MAX_SPEED) / max;
  }

  motor_speed[0] = motor_speed_final[0] = m100;
  motor_speed[1] = motor_speed_final[1] = m220;
  motor_speed[2] = motor_speed_final[2] = m340;

  //motor_set_speed_slew((signed char)m100, (signed char)m220,
  //    (signed char)m340);

}

void motor_set_speed_slew(signed char speed100, signed char speed220,
   signed char speed340) {

  motor_timeout_count = 0;
  
  if(speed100 < -MOTOR_MAX_SPEED)
    speed100 = -MOTOR_MAX_SPEED;
  if(speed100 > MOTOR_MAX_SPEED)
    speed100 = MOTOR_MAX_SPEED;
  if(speed220 < -MOTOR_MAX_SPEED)
    speed220 = -MOTOR_MAX_SPEED;
  if(speed220 > MOTOR_MAX_SPEED)
    speed220 = MOTOR_MAX_SPEED;
  if(speed340 < -MOTOR_MAX_SPEED)
    speed340 = -MOTOR_MAX_SPEED;
  if(speed340 > MOTOR_MAX_SPEED)
    speed340 = MOTOR_MAX_SPEED;

  if(speed100 == motor_speed_final[0] && speed220 == motor_speed_final[1] &&
      speed340 == motor_speed_final[2]) 
    return;

  motor_slew_steps = ABS(speed100 - motor_speed[0]);
  if(ABS(speed220 - motor_speed[1]) > motor_slew_steps)
    motor_slew_steps = ABS(speed220 - motor_speed[1]);
  if(ABS(speed340 - motor_speed[2]) > motor_slew_steps)
    motor_slew_steps = ABS(speed340 - motor_speed[2]);

  motor_slew_steps *= MOTOR_SLEW_RATE;
  motor_speed_final[0] = speed100;
  motor_speed_final[1] = speed220;
  motor_speed_final[2] = speed340;

  motor_slew_interval[0] = ((float)(speed100 - motor_speed[0])) / motor_slew_steps;
  motor_slew_interval[1] = ((float)(speed220 - motor_speed[1])) / motor_slew_steps;
  motor_slew_interval[2] = ((float)(speed340 - motor_speed[2])) / motor_slew_steps;
  motor_speed_float[0] = motor_speed[0];
  motor_speed_float[1] = motor_speed[1];
  motor_speed_float[2] = motor_speed[2];
  motor_slew_count = 0;
} 

unsigned char motor_get_voltage() {
  
  unsigned int temp;
 
	if(0 == (power_rx_data[0] & 0xFF))
		return 255;
  temp = power_rx_data[0] & 0xFF;
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

  temp = power_rx_data[0] & 0xFF;
  temp = ((temp + 148) * 7) / 27; 
  motor_tx_data[motor][0] = MOTOR_PWM_HEADER;
  motor_tx_data[motor][1] = pwm & 0xFF;
  motor_tx_data[motor][2] = temp & 0xFF;
}

signed char motor_clicks(int motor) {

  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return 0;

  return motor_rx_data[motor][1] & 0xFF;
}

unsigned char motor_current(int motor) {

  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return 0;

  return motor_rx_data[motor][2] & 0xFF;
}

unsigned char motor_temp(int motor) {
  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return 0;

  return motor_rx_data[motor][3] & 0xFF;
}

signed char motor_command(int motor) {
  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return 0;

  return motor_speed_final[motor];
}

