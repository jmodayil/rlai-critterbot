#include "lib_mi.h"
#include "lib_ui.h"
#include "armio.h"
#include "lib_motor.h"
#include "lib_adc.h"
#include "lib_adcspi.h"
#include "lib_accel.h"
#include "lib_events.h"
#include "lib_leddrive.h"
#include "lib_thermo.h"
#include "lib_monitor.h"

struct command_packet robot_command;
extern unsigned short crctable[256];

unsigned short crc;
unsigned char mi_test;

unsigned char mi_disabled_commands = 0;
unsigned char commands_packet_read = 0;

void mi_start(void) {
  leddrive_mi();
  ui_set_handler(mi_event);
  error_clear(0xFFFFFFFF);
}

void mi_stop(void) {
  leddrive_ui();
  ui_clear_handler(mi_event);
}

void mi_disable_commands() { 
  mi_disabled_commands = 1;
}

void mi_enable_commands() {
  mi_disabled_commands = 0;
}

int mi_event(void) {
  mi_get_commands();
  mi_send_status();
  error_clear(0xFFFFFFFF);
  return 0;
}

void putwcrc(unsigned char data) {
 
  crc = (crc<<8) ^ crctable[(crc >> 8) ^ data];
  armputchar(data);
}

void mi_send_status(void) {

  int i;
 
  crc = 0; 
  armputchar(MI_HEADER1);
  armputchar(MI_HEADER2);
  armputchar(MI_HEADER3);
  armputchar(MI_HEADER4);
  putwcrc(power_get_voltage());
  putwcrc(power_get_charge_state());
  putwcrc(power_get_bat40());
  putwcrc(power_get_bat160());
  putwcrc(power_get_bat280());
  for(i = 0; i < MOTOR_NUM_MOTORS; i++) {
    putwcrc(motor_voltage(i));
    putwcrc(motor_clicks(i));
    putwcrc(motor_current(i));
    putwcrc(motor_temp(i));
  }
  // Accelerometer
  putwcrc(accel_output[0] >> 4);
  putwcrc(accel_output[1] >> 4);
  putwcrc(accel_output[2] >> 4);
  // Least significant nibbles of the above three
  putwcrc((accel_output[0] << 4) | (accel_output[1] & 0x0F));
  putwcrc(accel_output[2] << 4);
  // Magnetometer
  putwcrc(adc_output[4] >> 2);
  putwcrc(adc_output[5] >> 2);
  putwcrc(adc_output[6] >> 2);
  // Gyroscope
  putwcrc((adcspi_get_output(3, 12) >> 2) - 128);
  // Least significant 2 bits from the above 4
  putwcrc(((adc_output[4] & 0x03) << 6) | 
      ((adc_output[5] & 0x03) << 4) |
      ((adc_output[6] & 0x03) << 2) | 
      (adcspi_get_output(3, 12) & 0x03));
  // IR Distance
  for(i = 0; i < 10; i++)
    putwcrc(adcspi_get_output(0, i) >> 2);
  // Least significant 2 bits from the above 10
  putwcrc(((adcspi_get_output(0, 0) & 0x03) << 6) |
      ((adcspi_get_output(0, 1) & 0x03) << 4) |
      ((adcspi_get_output(0, 2) & 0x03) << 2) |
      (adcspi_get_output(0, 3) & 0x03));
  putwcrc(((adcspi_get_output(0, 4) & 0x03) << 6) |
      ((adcspi_get_output(0, 5) & 0x03) << 4) |
      ((adcspi_get_output(0, 6) & 0x03) << 2) |
      (adcspi_get_output(0, 7) & 0x03));
  putwcrc(((adcspi_get_output(0, 8) & 0x03) << 6) |
      ((adcspi_get_output(0, 9) & 0x03) << 4));
  // Light sensors
  putwcrc(adcspi_get_output(3,8) >> 2);
  putwcrc(adcspi_get_output(3,9) >> 2);
  putwcrc(adcspi_get_output(3,10) >> 2);
  putwcrc(adcspi_get_output(3,11) >> 2);
  // Least significant bits from the above 4
  putwcrc(((adcspi_get_output(3, 8) & 0x03) << 6) |
      ((adcspi_get_output(3, 9) & 0x03) << 4) |
      ((adcspi_get_output(3, 10) & 0x03) << 2) |
      (adcspi_get_output(3, 11) & 0x03));
  // Thermal sensors
  for(i = 0; i < 8; i++) {
    putwcrc((unsigned char)(thermo_get_val(i)));
    putwcrc((unsigned char)(thermo_get_val(i)>>8));
  }
  // IR light sensors
  for(i = 0; i < 8; i++) {
    putwcrc((unsigned char)adcspi_get_output(3,i) >> 2);
  }
  // Least significant bits from the above 8
  putwcrc(((adcspi_get_output(3, 0) & 0x03) << 6) |
      ((adcspi_get_output(3, 1) & 0x03) << 4) |
      ((adcspi_get_output(3, 2) & 0x03) << 2) |
      (adcspi_get_output(3, 3) & 0x03));
  putwcrc(((adcspi_get_output(3, 4) & 0x03) << 6) |
      ((adcspi_get_output(3, 5) & 0x03) << 4) |
      ((adcspi_get_output(3, 6) & 0x03) << 2) |
      (adcspi_get_output(3, 7) & 0x03));
  putwcrc(error_reg >> 24);
  putwcrc(error_reg >> 16);
  putwcrc(error_reg >> 8);
  putwcrc(error_reg);
  putwcrc(commands_packet_read);
  putwcrc(monitor_status());

  armputchar(crc >> 8);
  armputchar(crc & 0xFF);
  return;
}

void mi_get_commands(void) {
  
  signed char m1, m2, m3;
  int i;

  
  ALIGNMENT_ERROR:
  commands_packet_read = 0;
  while (armgetnumchars() >= MI_MINIMUM_COMMAND_LENGTH) {
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
	  robot_command.avr_commands = armgetchar();

	  if(robot_command.led_mode == CCUSTOM && power_get_charge_state() == 0 &&
		!mi_disabled_commands) {
		for( i = 0; i < LED_NUM_LEDS; i++ ) {
		  LED[i].r = armgetchar();
		  LED[i].g = armgetchar();
		  LED[i].b = armgetchar();
		}
	  }
	  else {
		if (robot_command.led_mode == CCUSTOM) {
		  for( i = 0; i < LED_NUM_LEDS * 3; i++ ) {
			armgetchar();
		  }
		}
	  }
	  commands_packet_read++;
  }
  if (!commands_packet_read)
	  return;
  if (!mi_disabled_commands) {
    switch(robot_command.motor_mode) {
      // The various minus signs here are to correct the coordinate system.
      // Really this is an easy way to do it, and should be corrected both
      // in the signs of the XYT->Wheel transform and the wheel drivers
      // themselves.
      case WHEEL_SPACE:
        motor_set_speed_slew(m1, m2, m3);
        break;
      case XYTHETA_SPACE:
        motor_set_speed_xytheta(m1, m2, m3);
        break;
      case WHEEL_VOLTAGE:
        motor_set_voltage(m1, m2, m3);
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
  
    if(power_get_charge_state() == 0) { 
      switch(robot_command.led_mode) {
        case CNONE:
          break;
        case CCLEAR:
          leddrive_clear();
          break;
        case CBATTERY:
          leddrive_batstatus();
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
          break;
        case CCUSTOM:
          leddrive_custom(); 
          break;
        default:
          break;
      }
    }

    // Enabling charging has priority over disabling
    if (robot_command.avr_commands & MI_AVR_ENABLE_CHARGING)
      motor_enable_charging();
    else if (robot_command.avr_commands & MI_AVR_DISABLE_CHARGING)
      motor_disable_charging();
  }
  else { // mi_disabled_commands
    if(robot_command.motor_mode == MOTOR_EXIT && 
       robot_command.led_mode == LED_EXIT)
      mi_stop();
  }
  return;
}
