#include "lib_mi.h"
#include "lib_ui.h"
#include "armio.h"
#include "lib_motor.h"
#include "lib_adc.h"
#include "lib_adcspi.h"
#include "lib_accel.h"
#include "lib_events.h"

struct command_packet robot_command;

void mi_start(void) {
  ui_set_handler(mi_event);
}

void mi_stop(void) {
  ui_clear_handler(mi_event);
}

int mi_event(void) {

  mi_send_status();
  mi_get_commands();

  return 0;
}

void mi_send_status(void) {

  int i;

  armputchar(MI_HEADER1);
  armputchar(MI_HEADER2);
  armputchar(MI_HEADER3);
  armputchar(MI_HEADER4);
  for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
    armputchar(motor_clicks(i));
    armputchar(motor_current(i));
    armputchar(motor_temp(i));
  }
  armputchar(accel_output[0] >> 4);
  armputchar(accel_output[1] >> 4);
  armputchar(accel_output[2] >> 4);
  armputchar(adc_output[0] >> 2);
  armputchar(adc_output[1] >> 2);
  armputchar(adc_output[3] >> 2);
  armputchar(adcspi_get_output(12) >> 4);
  for(i = 0; i < 10; i++)
    armputchar(0);
  for(i = 0; i < 4; i++)
    armputchar(3);
  armputchar(error_reg >> 24);
  armputchar(error_reg >> 16);
  armputchar(error_reg >> 8);
  armputchar(error_reg);
  armputchar(events_time());
}

void mi_get_commands(void) {
  
  int max;
  int xvel, yvel, tvel;
  int m100, m220, m340;
  
  ALIGNMENT_ERROR:

  if(MI_COMMAND_LENGTH > armgetnumchars())
    return;
    
  if(MI_HEADER1 != armgetchar())
    goto ALIGNMENT_ERROR;
  if(MI_HEADER2 != armgetchar())
    goto ALIGNMENT_ERROR;
  if(MI_HEADER3 != armgetchar())
    goto ALIGNMENT_ERROR;
  if(MI_HEADER4 != armgetchar())
    goto ALIGNMENT_ERROR;
  
  robot_command.motor_mode = armgetchar();
  switch(robot_command.motor_mode) {
    case WHEEL_SPACE:
      motor_set_speed(0, (char)armgetchar());
      motor_set_speed(1, (char)armgetchar());
      motor_set_speed(2, (char)armgetchar());
      break;
    case XYTHETA_SPACE:
      xvel = (char)armgetchar();
      yvel = (char)armgetchar();
      tvel = (char)armgetchar();
  
      m100 = (xvel * XSC100 + yvel * YSC100 + tvel * TSC100) / 1024;
      m220 = (xvel * XSC220 + yvel * YSC220 + tvel * TSC220) / 1024;
      m340 = (xvel * XSC340 + yvel * YSC340 + tvel * TSC340) / 1024;  

      /*max = ABS(m100);
      if(ABS(m220) > max)
        max = ABS(m220);
      if(ABS(m340) > max)
        max = ABS(m340);
      if(max > MOTOR_MAX_SPEED) {
        m100 = (m100 * MOTOR_MAX_SPEED) / max;
        m220 = (m220 * MOTOR_MAX_SPEED) / max;
        m340 = (m340 * MOTOR_MAX_SPEED) / max;
      }*/
      motor_set_speed(0, (char)m100);
      motor_set_speed(1, (char)m220);
      motor_set_speed(2, (char)m340);
      break;
    default:
      robot_command.motor_mode = WHEEL_SPACE;
      motor_set_speed(0, 0); 
      motor_set_speed(1, 0);
      motor_set_speed(2, 0);
      break;
  }
  robot_command.led_mode = armgetchar();
  return;
}
