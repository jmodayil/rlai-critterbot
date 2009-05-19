#include "lib_mi.h"
#include "lib_ui.h"
#include "armio.h"
#include "lib_motor.h"
#include "lib_adc.h"
#include "lib_adcspi.h"
#include "lib_accel.h"
#include "lib_events.h"
#include "lib_leddrive.h"

event_s mi_recv_event_s = {
  NULL,
  mi_get_commands,
  0
};

event_s mi_send_event_s = {
  NULL,
  mi_send_status,
  0
};

struct command_packet robot_command;
extern unsigned short crctable[256];

unsigned short crc;
unsigned char mi_test;

void mi_start(void) {
  leddrive_clear();
  event_start(EVENT_ID_MI_SEND);
  event_start(EVENT_ID_MI_RECV);
  event_stop(EVENT_ID_UI);
  error_clear(0xFFFFFFFF);
}

void mi_stop(void) {
  leddrive_ball();
  event_stop(EVENT_ID_MI_SEND);
  event_stop(EVENT_ID_MI_RECV);
  event_start(EVENT_ID_UI);
}

void putwcrc(unsigned char data) {
 
  crc = (crc<<8) ^ crctable[(crc >> 8) ^ data];
  armputchar(data);
}

int mi_send_status(void) {

  int i;
 
  crc = 0; 
  armputchar(MI_HEADER1);
  armputchar(MI_HEADER2);
  armputchar(MI_HEADER3);
  armputchar(MI_HEADER4);
  putwcrc(motor_get_voltage());
  for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
    putwcrc(motor_command(i));
    putwcrc(motor_clicks(i));
    putwcrc(motor_current(i));
    putwcrc(motor_temp(i));
  }
  putwcrc(accel_output[0] >> 4);
  putwcrc(accel_output[1] >> 4);
  putwcrc(accel_output[2] >> 4);
  //putwcrc(adc_output[0] >> 2);
  //putwcrc(adc_output[1] >> 2);
  //putwcrc(adc_output[3] >> 2);
  putwcrc((adcspi_get_output(3, 12) >> 2) - 128);
  for(i = 0; i < 10; i++)
    putwcrc(adcspi_get_output(0, i) >> 2);
  putwcrc(adcspi_get_output(3,8) >> 2);
  putwcrc(adcspi_get_output(3,9) >> 2);
  putwcrc(adcspi_get_output(3,10) >> 2);
  putwcrc(adcspi_get_output(3,11) >> 2);
  putwcrc(error_reg >> 24);
  putwcrc(error_reg >> 16);
  putwcrc(error_reg >> 8);
  putwcrc(error_reg);
  putwcrc(events_time());

  armputchar(crc >> 8);
  armputchar(crc & 0xFF);
  return 0;
}

int mi_get_commands(void) {
  
  signed char m1, m2, m3;
  int i;
  
  ALIGNMENT_ERROR:

  if(MI_COMMAND_LENGTH > armgetnumchars())
    return 0;
    
  if(MI_HEADER1 != armgetchar())
    goto ALIGNMENT_ERROR;
  if(MI_HEADER2 != armgetchar())
    goto ALIGNMENT_ERROR;
  if(MI_HEADER3 != armgetchar())
    goto ALIGNMENT_ERROR;
  if(MI_HEADER4 != armgetchar())
    goto ALIGNMENT_ERROR;
  
  robot_command.motor_mode = armgetchar();
  m1 = ((signed char)((unsigned char)armgetchar()));
  m2 = ((signed char)((unsigned char)armgetchar()));
  m3 = ((signed char)((unsigned char)armgetchar()));
  robot_command.led_mode = armgetchar();
  
  if(robot_command.led_mode == CCUSTOM) { 
    for( i = 0; i < LED_NUM_LEDS; i++ ) {
      LED[i].r = armgetchar();
      LED[i].g = armgetchar();
      LED[i].b = armgetchar();
    }
  }  
  else {
    for( i = 0; i < LED_NUM_LEDS * 3; i++ ) {
      armgetchar();
    }
  }
  
  switch(robot_command.motor_mode) {
    case WHEEL_SPACE:
      motor_set_speed_slew(m1, m2, m3);
      break;
    case XYTHETA_SPACE:
      motor_set_speed_xytheta(m1, m2, m3);
      break;
    case MOTOR_EXIT:
      if(robot_command.led_mode == LED_EXIT)
        mi_stop();
      break;
    default:
      robot_command.motor_mode = WHEEL_SPACE;
      motor_set_speed(0, 0); 
      motor_set_speed(1, 0);
      motor_set_speed(2, 0);
      break;
  }
 
  switch(robot_command.led_mode) {
    case CNONE:
      break;
    case CCLEAR:
      leddrive_clear();
      break;
    case CBATTERY:
      leddrive_rainbow();
      break;
    case CBALL:
      leddrive_ball();
      break;
    case CERROR:
      leddrive_error();
      break;
    case CBUSY:
      leddrive_busy();
      break;
    case CEMERGENCY:
      leddrive_emerg();
    case CCUSTOM:
      leddrive_custom(); 
      break;
    default:
      break;
  }
  return 0;
}