/*
 * lib_motor.h
 *
 * Created by Michael Sokolsky
 * Last modified: 30 April 09
 */

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
  
  power_init_packet();

  return 0;  
}

int motor_event() {
  
  unsigned int volt;
  int i;

  // Stop moving if we haven't received a command for a while
  if(++motor_timeout_count == MOTOR_TIMEOUT) {
    motor_set_speed_slew(0,0,0);
    motor_timeout_count = 0;
  }

  // While we are slewing to a new motor speed
  if(motor_slew_count < motor_slew_steps) {
    for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
      // Increment the float approximate value and then set as speed
      motor_speed_float[i] += motor_slew_interval[i];
      motor_speed[i] = (signed char) motor_speed_float[i];
    }
    motor_slew_count++;  
  }
  // Once we have read a steady speed
  else if(motor_slew_count == motor_slew_steps) {
    for(i = 0; i < MOTOR_NUM_MOTORS; i++)
      motor_speed[i] = (signed char)  motor_speed_final[i];
    motor_slew_count++;
  }

  // What was the bus voltage on the last cycle?
  volt = motor_get_voltage();
  // Now we queue the new commands to the motors.
  spi_send_packet(&power_packet); 
  for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
    motor_tx_data[i][1] = (unsigned char) motor_speed[i];
    motor_tx_data[i][2] = volt;  
    spi_send_packet(&motor_packet[i]);
  }
  // This checks the old packet we received to make sure it is a proper packet
  for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
    if((motor_rx_data[i][4] & 0xFF) != MOTOR_SPI_PADDING)
      error_set(ERR_MOTOR_ALIGN);
  }
  if((power_rx_data[2] & 0xFF) != MOTOR_SPI_PADDING)
    error_set(ERR_MOTOR_ALIGN);
  return 0; 
}

void motor_set_speed(int motor, signed char speed) {

  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return;

  if(speed < -MOTOR_MAX_SPEED)
    speed = -MOTOR_MAX_SPEED;
  if(speed > MOTOR_MAX_SPEED) 
    speed = MOTOR_MAX_SPEED;

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

  //motor_speed[0] = motor_speed_final[0] = m100;
  //motor_speed[1] = motor_speed_final[1] = m220;
  //motor_speed[2] = motor_speed_final[2] = m340;

  motor_set_speed_slew((signed char)m100, (signed char)m220,
      (signed char)m340);

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

/*
 * Take the raw ADC value received from the power controller and convert it to
 * useful numbers (1/10th of a Volt)
 */
unsigned char motor_get_voltage() {
  
  unsigned int temp;
 
  temp = power_rx_data[1] & 0xFF;
	// 0 is not a valid voltage, and it also may indicate a communication error
  // with the power controller.  If this is the case we don't actually know
  // what the bus voltage is, so set it as high as possible for motor safety.
  if(0 == temp)
		return 255;
  return (temp*100 + 14730) / 154;
}

void motor_init_packet(int motor) {

  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return;

  motor_tx_data[motor][0] = MOTOR_PACKET_HEADER;
  motor_tx_data[motor][1] = 0;
  motor_tx_data[motor][2] = 255;
  motor_tx_data[motor][3] = 0;
  motor_tx_data[motor][4] = 0;

}

void power_init_packet() {

  power_tx_data[0] = MOTOR_PACKET_HEADER;
  power_tx_data[1] = 0;
  power_tx_data[2] = 0;
}

/*
 * Not yet implemented on the controller side, however this should send raw
 * PWM values to the controllers for low-level control
 */
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

/*
 * Returns the current velocity of the motor in encoder clicks per PID cycle
 */
signed char motor_clicks(int motor) {

  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return 0;

  return motor_rx_data[motor][1] & 0xFF;
}

/*
 * Returns the current amperage of the motor in unknown units
 */
unsigned char motor_current(int motor) {

  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return 0;

  return motor_rx_data[motor][2] & 0xFF;
}

/*
 * Returns the current temperature of the motor housing, in unknown units.
 * This value has an inverse relationship with temperature.
 */
unsigned char motor_temp(int motor) {
  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return 0;

  return motor_rx_data[motor][3] & 0xFF;
}

/*
 * Returns the last motor command received
 */
signed char motor_command(int motor) {
  if(motor < 0 || motor >= MOTOR_NUM_MOTORS)
    return 0;

  return motor_speed_final[motor];
}

