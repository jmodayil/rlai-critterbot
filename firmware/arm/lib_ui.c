/*
 *  lib_ui.c
 *  
 *
 *  Created by Marc G. Bellemare on 27/06/08.
 *  Last Revision: 1/07/08
 *
 */

#include "AT91SAM7S256.h"
#include "compiler.h"
#include "lib_ui.h"
#include "lib_ledctl.h"
#include "lib_ssc.h"
#include "lib_accel.h"
#include "armio.h"
#include "lib_error.h"
#include "lib_boot.h"
#include "lib_critical.h"
#include "lib_leddrive.h"
#include "lib_events.h"
#include "lib_adcspi.h"
#include "lib_adc.h"
#include "lib_mi.h"
#include "lib_thermo.h"
#include "lib_recharger.h"

#define INVALID_RANGE_STR ("%s must be in range %d..%d\r")
#define INVALID_ARGNUM_STR ("Invalid argument(s) for %s.\r")

// Included for EOF, NULL
#include <stdio.h>

event_s ui_event_s = {
  ui_init,
  ui_event,
  0
};

extern volatile int motor_test;
#define STATUS_STRING(a) ( (a)? "OK" : "FAIL" )

/* Structure used to hold command name + function pairs */
typedef struct {
  char * name;
  ui_function func;
  char * description;
} ui_cmd_item;

extern struct ssc_packet ledctl_ssc_packet[];

extern volatile int motor_test;

ui_cmd_item ui_commands[] = {
  { "help", ui_help, "Do you need help?"}, // help
  { "set_led", ui_setled, "set_led <led #> <red> <green> <blue>"}, 
    // set_led <led num, 0-15> <red> <green> <blue>
  { "get_led", ui_getled, "get_led <led #>"},
    // get_led <led_num, 0-15>
  {"stat_led", ui_statled, "stat_led"},
  {"clearall", ui_clearall, "clear"},
  {"setall", ui_setall, "setall"},
  {"set_dot", ui_setdot, "set_dot <led #> <red> <green> <blue>"},
  {"get_dot", ui_getdot, "get_dot [led #] - argument optional"},
  {"get_accel", ui_getaccel, "get_accel"},
  {"get_adcspi", ui_getadcspi, "get_adcspi"},
  {"toggle_adcspi", ui_toggle_adcspi, "toggle_adcspi"},
  {"status", ui_status, "status"},
  {"report", ui_report, "report [led|accel|adc0-4|error]"},
  {"mode", ui_mode, "mode <led [gs|dc]> - broken"},
  {"test", ui_test, "test [ramfunc|int|stress]"},
  {"bootloader", ui_bootloader, "bootloader [program] - reprogram the chip"},
  {"reset", ui_reset, "reset"},
  {"pid", ui_pid, "pid [start|stop|stat] #"},
  {"error", ui_error, "error [clear]"},
  {"motor", ui_motor, "motor [motor #] [speed #], or\r      [motor1speed] [motor2speed] [motor3speed]"},
  {"mi", ui_mi, "mi on"},
  {"recharge", ui_recharge, "recharge [start|stop]"},
  {"avr", ui_avr, "avr [command]"},
};

int ui_ncommands = sizeof(ui_commands)/sizeof(*ui_commands);

char ui_command_string[256];
char ui_cmdname[64];
char ui_strarg[64];

volatile ui_io_handler_fn ui_io_handler = NULL;

// Reporting mode - if nonzero, the UI will spit out some status information
//  every so often.
volatile int ui_report_mode = 0;
volatile int ui_report_clock = 0;
volatile int ui_report_list = UI_REPORT_DEFAULT;

/** Finds the requested command in our list of commands.
  * Returns the corresponding item if found, NULL otherwise.
  */

ui_cmd_item * ui_parse_command(char * cmdstr)
{
  int idx;
  if(armsscanf(cmdstr, "%s", ui_cmdname) < 1) return NULL;
  for (idx = 0; idx < ui_ncommands; idx++)
    if (strncmp(ui_commands[idx].name, ui_cmdname, sizeof(ui_cmdname)) == 0)
      return &ui_commands[idx];
  return NULL;
}

int ui_set_handler(ui_io_handler_fn handler)
{
  if (ui_io_handler != NULL) 
    return 0;
  else
  {
    ui_io_handler = handler;
    return 1;
  }
}

void ui_clear_handler(ui_io_handler_fn handler)
{
  // The case when handler == NULL is still quite fine
  if (ui_io_handler == handler)
    ui_io_handler = NULL;
}

int ui_init()
{
  // This is not strictly necessary if the UI is only initialized once
  //  However, if we want to re-initialize the UI, we should clear these flags
  ui_io_handler = NULL;
  ui_report_mode = 0;
  ui_report_clock = 0;

  return 0;
}

int ui_event()
{
  
  // If some task has requested to be in charge of the serial IO, the UI
  //  will completely stop its operation until the ui_io_handler is cleared.
  if (ui_io_handler != NULL)
    return ui_io_handler();
  
  // Report if in reporting mode
  if (ui_report_mode)
  {
    if (++ui_report_clock >= UI_REPORT_INTERVAL)
    {
      ui_do_report();
      ui_report_clock = 0;
    }
  }

  // Check whether we have new data
  if (armreadline(ui_command_string, sizeof(ui_command_string)) == EOF)
    return 0;
  armputchar('\r');
  // Parse command, return false if not found
  ui_cmd_item * cmd = ui_parse_command(ui_command_string);
  // Not found, write error message
  if (cmd == NULL)
    armprintf ("Invalid command: \"%s\"\r", ui_command_string);
  else
  {
    // Call function
    (*(cmd->func))(ui_command_string); 
  }
  armprintf("critterprompt> ");
  return 0;
}

void ui_do_report()
{
  int i;
  // The lazy way - call other status-related functions from the UI
  if (ui_report_list & UI_REPORT_LED)
    ui_statled("stat_led");
  if (ui_report_list & UI_REPORT_ACCEL)
    ui_getaccel("get_accel");
  // @@@ Fixme
  if (ui_report_list & UI_REPORT_ADC0)
    ui_getadcspi("get_adcspi");
  if (ui_report_list & UI_REPORT_ADC_ONBOARD)
  {
    for(i = 4; i < 8; i++)
      armprintf("ADC Channel %d: %d\r", i, adc_output[i]);
  }
  if (ui_report_list & UI_REPORT_ERROR)
    armprintf ("Error status: %x\r", error_get());
}

void ui_help(char * cmdstr)
{
  int idx;
  
  armprintf ("List of commands and usage: \r");

  for (idx = 0; idx < ui_ncommands; idx++)
    armprintf ("%s - %s\r", ui_commands[idx].name, 
      ui_commands[idx].description);
}

void ui_setled(char * cmdstr)
{
  int ledNum;
  int ledColors[3];

  // parse in num, color
  if (armsscanf (cmdstr, "%s %d %d %d %d", ui_cmdname,
    &ledNum, &ledColors[0], &ledColors[1], &ledColors[2]) < 5)
  {
    armprintf (INVALID_ARGNUM_STR, "set_led");
    return;
  }

  if (ledNum < 0 || ledNum >= LEDCTL_NUM_LEDS ||
      ledColors[0] < 0 || ledColors[1] < 0 || ledColors[2] < 0 ||
      ledColors[0] >= LEDCTL_MAX_VALUE || 
      ledColors[1] >= LEDCTL_MAX_VALUE ||
      ledColors[2] >= LEDCTL_MAX_VALUE)
  {
    armprintf (INVALID_RANGE_STR, "LED", 0, LEDCTL_NUM_LEDS);
    armprintf (INVALID_RANGE_STR, "Colors", 0, LEDCTL_MAX_VALUE); 
    return;
  }

  // Set the LED's three colors
  ledctl_setcolor(ledNum, ledColors[0], ledColors[1], ledColors[2]);
}

/** Returns the status of a given led (0-15) */
void ui_getled (char * cmdstr)
{
  int ledNum;
  int ledColors[3]; 
  int color;

  if (armsscanf (cmdstr, "%s %d", ui_cmdname, &ledNum) < 2)
  {
    armprintf (INVALID_ARGNUM_STR, "get_led");
    return;
  }

  if (ledNum < 0 || ledNum >= LEDCTL_NUM_LEDS)
  {
    armprintf (INVALID_RANGE_STR, "LED", 0, LEDCTL_NUM_LEDS);
    return;
  }
  
  for (color = 0; color < 3; color++)
    ledColors[color] = ledctl_getcolor(ledNum, color);

  armprintf ("LED %d: %d,%d,%d\r", ledNum, 
    ledColors[0], ledColors[1], ledColors[2]);
}

void ui_statled(char * cmdstr)
{
  int i;

  armprintf ("XERR status: %d\r", ledctl_geterr());
  for (i = 0; i < LEDCTL_NUM_CONTROLLERS; i++)
    armprintf ("LOD %d status: %x\r", i, ledctl_getLOD(i));
  armprintf ("TEF status: %x\r", ledctl_getTEF());

  armprintf ("If LOD or TEF != 0 and XERR is on or vice-versa, "
    "report to MGB.\r");
  armprintf ("\r");
}

void ui_status(char * cmdstr)
{
  unsigned int reset_code;
  armprintf("Last reset was a ");
  reset_code = get_reset_code();
  if (reset_code >= 0 && reset_code < num_events_reset_names)
    armprintf(events_reset_names[reset_code]); 
  else
    armprintf(events_undefined_reset_name);
  armprintf(" reset.\r");
  armprintf ("Event prior to reset was: %d\r", pre_reset_event);
  //armprintf ("LED status: %s\r", STATUS_STRING(!ledctl_geterr()));
  armprintf ("Accelerometer status: %s\r", "N/A");
  armprintf ("Error status: %x\r", error_get());
  armprintf ("Thermos: %d %d %d %d\r", thermo_get_val(0), thermo_get_val(2),
     thermo_get_val(4), thermo_get_val(6));
  //   adc_output[2], adc_output[3]); 
  //armprintf ("SPI Status Register: %x\r", AT91C_BASE_SPI->SPI_SR);
  //armprintf ("SPI PDC Status: %x\r", AT91C_BASE_SPI->SPI_PTSR);
  armprintf ("On board adc: %d %d %d %d\r", adc_output[4], adc_output[5],
     adc_output[6], adc_output[7]); 
  armprintf ("System Voltage: %d\r", motor_get_voltage());
  armprintf ("Charge State: %d\r", motor_get_charge_state());
  armprintf ("Battery Voltages: %d %d %d\r", motor_get_bat40(),
      motor_get_bat160(), motor_get_bat280());
  armprintf ("Motor Speeds: %d %d %d\r", motor_clicks(0), 
      motor_clicks(1), motor_clicks(2));
  armprintf ("AVR control: %s %s\r", 
    motor_is_drive_enabled()?"":"disabled drive",
    motor_is_charging_enabled()?"":"disabled charging");
  armprintf("\r");
}

void ui_clearall(char * cmdstr)
{
  int l;

  for (l = 0; l < LEDCTL_NUM_LEDS; l++)
    ledctl_setcolor(l, 0, 0, 0);
}

void ui_setall(char * cmdstr)
{
  int colValue;
  int l;

  if (armsscanf (cmdstr, "%s %d", ui_cmdname, &colValue) < 2)
  {
    armprintf (INVALID_ARGNUM_STR, "setall");
    return;
  }

  for (l = 0; l < LEDCTL_NUM_LEDS; l++)
    ledctl_setcolor(l, colValue, colValue, colValue);
}

/** Similar to ui_setled, except that it sets dot correction data */
void ui_setdot(char * cmdstr)
{
  int ledNum;
  int ledColors[3];

  // parse in num, color
  if (armsscanf (cmdstr, "%s %d %d %d %d", ui_cmdname,
    &ledNum, &ledColors[0], &ledColors[1], &ledColors[2]) < 5)
  {
    armprintf (INVALID_ARGNUM_STR, "set_dot");
    return;
  }

  if (ledNum < 0 || ledNum >= LEDCTL_NUM_LEDS ||
      ledColors[0] < 0 || ledColors[1] < 0 || ledColors[2] < 0 ||
      ledColors[0] >= LEDCTL_DC_MAX_VALUE || 
      ledColors[1] >= LEDCTL_DC_MAX_VALUE ||
      ledColors[2] >= LEDCTL_DC_MAX_VALUE)
  {
    armprintf (INVALID_RANGE_STR, "LED", 0, LEDCTL_NUM_LEDS);
    armprintf (INVALID_RANGE_STR, "Colors", 0, LEDCTL_MAX_VALUE);
    return;
  }

  // Set the LED's three colors
  ledctl_setdc(ledNum, ledColors[0], ledColors[1], ledColors[2]);

  // Re-apply dot-correction right away
  armprintf ("Applying dot correction...");
  // ledctl_dc();
  armprintf ("Not yet! Use mode led dc.\r");
}

/** Returns the DC of a given led (0-15) */
void ui_getdot (char * cmdstr)
{
  int ledNum;
  int ledColors[3]; 
  int color;
  int numArgs;

  numArgs = armsscanf (cmdstr, "%s %d", ui_cmdname, &ledNum);
  // numArgs == 1 implies get all dc data
  if (numArgs < 2)
    ledNum = 0;
  else if (ledNum < 0 || ledNum >= LEDCTL_NUM_LEDS)
  {
    armprintf (INVALID_RANGE_STR, "LED", 0, LEDCTL_NUM_LEDS);
    return;
  }

  while (ledNum < LEDCTL_NUM_LEDS)
  {
    for (color = 0; color < 3; color++)
      ledColors[color] = ledctl_getdc(ledNum, color);
    armprintf ("LED %d dot correction: %d,%d,%d\r", ledNum, 
      ledColors[0], ledColors[1], ledColors[2]);
    if (numArgs == 1)
      ledNum++;
    else
      break;
  }

}

void ui_getaccel (char * cmdstr)
{
  // Report on the three axes & status
  int i; 
  unsigned char status = accel_get_status();

  armprintf ("Accelerometer status: %d ", status);
  for (i = 0; i < ACCEL_NUM_AXES; i++)
    armprintf (" %d", accel_get_output(i));
  armprintf ("\r");
}

void ui_getadcspi (char * cmdstr)
{
  int i,j;

  armprintf ("Off-board ADC outputs: \r");
  
  for(i = 0; i < ADCSPI_NUM_DEVICES; i++) {
    armprintf("ADC%d: ", i);
    for(j = 0; j < ADCSPI_OUTPUTS_PER_DEVICE; j++) {
      armprintf("%d ", adcspi_get_output(i,j));
    }
    armprintf("\r");
  }
}

void ui_toggle_adcspi(char * cmdstr)
{
/*  int index;
  
  if (armsscanf(cmdstr, "%s %d", ui_cmdname, &index) < 2)
  {
    armprintf ("Invalid argument to adcspi_toggle.\r");
    return;
  }

  if (adcspi_is_selected(index))
    adcspi_deselect(index);
  else
    adcspi_select(index);*/
}

void ui_report (char * cmdstr)
{
  // Toggle reporting mode
  if (armsscanf (cmdstr, "%s %s", ui_cmdname, ui_strarg) < 2)
  {
    ui_report_mode ^= 0x1;
    if (!ui_report_mode)
      ui_report_clock = 0;
  }
  else // Parse argument
  {
    if (strncmp(ui_strarg, "led", sizeof(ui_strarg)) == 0)
      ui_report_list ^= UI_REPORT_LED;
    else if (strncmp(ui_strarg, "accel", sizeof(ui_strarg)) == 0)
      ui_report_list ^= UI_REPORT_ACCEL;
    else if (strncmp(ui_strarg, "adc0", sizeof(ui_strarg)) == 0)
      ui_report_list ^= UI_REPORT_ADC0;
    else if (strncmp(ui_strarg, "adc1", sizeof(ui_strarg)) == 0)
      ui_report_list ^= UI_REPORT_ADC1;
    else if (strncmp(ui_strarg, "adc2", sizeof(ui_strarg)) == 0)
      ui_report_list ^= UI_REPORT_ADC2;
    else if (strncmp(ui_strarg, "adc3", sizeof(ui_strarg)) == 0)
      ui_report_list ^= UI_REPORT_ADC3;
    else if (strncmp(ui_strarg, "adc4", sizeof(ui_strarg)) == 0)
      ui_report_list ^= UI_REPORT_ADC_ONBOARD;
    else if (strncmp(ui_strarg, "error", sizeof(ui_strarg)) == 0)
      ui_report_list ^= UI_REPORT_ERROR;
    else
    {
      armprintf ("Unknown report element: %s\r", ui_strarg);
    }
  }
}

void ui_mode (char * cmdstr)
{
  char modestr[64];

  if (armsscanf(cmdstr, "%s %s %s", ui_cmdname, ui_strarg, modestr) < 3)
  {
    armprintf (INVALID_ARGNUM_STR, "ui_mode");
    return;
  }

  // Set the LED mode
  if (strncmp(ui_strarg, "led", sizeof(ui_strarg)) == 0)
  {
    if (strncmp (modestr, "gs", sizeof(modestr)) == 0)
      ledctl_setmode(FIRST_GRAYSCALE);
    else if (strncmp(modestr, "dc", sizeof(modestr)) == 0)
      ledctl_setmode(FIRST_DOT_CORRECTION);
    else
      armprintf ("Valid LED modes are: gs, dc.\r");
  }
  else
    armprintf ("Valid modes are: led.\r");
}


void ui_bootloader(char * cmdstr)
{
  unsigned int data_size;

  /* As 'status' report, tell the user if the bootloader core is not in RAM
  if (((char *)boot_core) < AT91C_ISRAM)
  {
    armprintf ("boot_core not in RAM\r");
    error_set (ERR_BOOT);
    return;
  }*/

  // DON'T REMOVE THIS
  // Prevents a very fringe case that could accidently re-flash chip
  *ui_strarg = '\0';
  
  if (armsscanf(cmdstr, "%s %s %d", ui_cmdname, ui_strarg, &data_size) < 3)
  {
    // BE CAREFUL CHANGING THIS CODE - FLASHES CHIP
    if(strncmp(ui_strarg, "program", sizeof(ui_strarg)) == 0) {
      if(boot_data_size > 0)
        boot_core();
    }
    armprintf ("Use 'bootloader program' to flash chip\r");
    return;
  }
  
  // Test for password, parse data size
  if (strncmp(ui_strarg, BOOT_PASSWORD, sizeof(ui_strarg)) != 0)
  {
    armprintf ("Wrong password.\r");
    error_set (ERR_BOOT);
    return;
  }

  // Unlikely data size or too big; we could make 0 a threshold (e.g. <= 500)
  if (data_size <= 0 || data_size >= BOOT_MAX_CODE_SIZE)
  {
    armprintf ("Data size is wrong: %d\r", data_size);
    error_set(ERR_BOOT);
    return;
  }

  boot_begin_receive(data_size);
}

void ui_reset(char * cmdstr)
{
  if (armsscanf(cmdstr, "%s %s", ui_cmdname, ui_strarg) < 2)
  {
    armprintf ("Usage: reset %s\r", UI_RESET_PASSWORD);
    return;
  }
 
  if (strncmp(ui_strarg, UI_RESET_PASSWORD, sizeof(ui_strarg)) != 0)
  {
    armprintf ("Wrong password (try %s).\r", UI_RESET_PASSWORD);
    return;
  }

  boot_reset_arm();
}

RAMFUNC void ui_test_ramfunc(char * cmdstr)
{
  armprintf (cmdstr);
  armprintf ("\r");
}

/** This tests the interrupt disable as follows:
  * Disable the interrupt processor line
  * Wait for the PIT to count over two event cycles
  * Re-enable the interrupt procesor line
  *
  * To use this, make sure that EVENTSLOW has not yet occured. The routine
  *  will block for about 20ms (2 cycles) to allow the PIT to count at least
  *  two cycles. After the test routine completes, you should return to
  *  normal functioning; if not, then re-enabling the interrupts failed
  *  (at that point nothing will work).
  */
void ui_test_int(char * cmdstr)
{
  unsigned int err_status = error_get();
  // We should use __armprintf here because the PDC interrupts will become 
  //  disabled
  armprintf ("Disabling interrupts: we will cause an EVENTSLOW error\r");
  armprintf ("Current error status: %x\r", err_status);
  if (err_status & ERR_EVENTSLOW)
    armprintf ("ERR_EVENTSLOW already set; test is meaningless\r");

  crit_disable_int();
  while (AT91C_BASE_PITC->PITC_PIIR < 0x00200000) ;
  crit_enable_int();
  
  err_status = error_get();
  if (err_status & ERR_EVENTSLOW)
    armprintf ("Event slow reported - interrupts were disabled.\r");
  else
    armprintf ("Interrupts were most likely NOT disabled.\r");
}

// Some dummy variables we might need for testing the serial IO
int ui_stress_data[4];

#define UI_STRESS_HANDLER1_TIMEOUT    EVENTS_HZ

/** Stress test #1, serial IO handler 
  * Prints out everything that is received until n lines have been read,
  *  where n is ui_stress_data[0]. Same handler for stress tests of type 1
  *  and 2. ui_stress_data[2] contains the stress test type. */
int ui_stress_handler1()
{
  // Read a full line
  if (armreadline(ui_command_string, sizeof(ui_command_string)) == EOF)
  {
    if (--ui_stress_data[1] <= 0)
    {
      armprintf ("UI stress handler 1 timeout!\n");
      error_set(ERR_SELF_TEST);
      ui_clear_handler(ui_stress_handler1);
      return 1;
    }
    return 0;
  }

  // Reset the timeout counter
  ui_stress_data[1] = UI_STRESS_HANDLER1_TIMEOUT;

  switch (ui_stress_data[2])
  {
    case 1: // RXTX test
      armprintf (ui_command_string);
      break;
    case 2: // printf test
      armprintf (ui_command_string, 
        0x1, 0x4, 0x10, 0x40, 0x100, 0x400, 0x1000, 0x4000,
        0x10000, 0x40000, 0x100000, 0x400000,
        0x1000000, 0x4000000, 0x10000000, 0x40000000);
      break;
  }

  // If we have received n lines, return the control to the UI
  if (--ui_stress_data[0] <= 0)
    ui_clear_handler(ui_stress_handler1);
  
  return 0;
}

void ui_test_stress(char * cmdstr)
{
  int stress_type;
  unsigned int data_length;
  unsigned int i;
  unsigned int root;

  if (armsscanf(cmdstr, "%s %s %d %u", ui_cmdname, ui_strarg, &stress_type) < 3)
  {
    armprintf ("Usage: test stress <type> <argument>\r");
    return;
  }
  root = 1;

  switch (stress_type)
  {
    case 0: 
      // random (pre-defined) chars; stress program should compare the 
      //  received sequence, which it can reproduce
      if (armsscanf(cmdstr, "%s %s %d %u", ui_cmdname, ui_strarg, 
        &stress_type, &data_length) < 4)
      {
        data_length = 0;
      }
      
      for (i = 0; i < data_length; i++)
      {
        // Root \in 1..256
        armputchar((char)(root % 96 + 31));
        // Not quite random, but this is a group of order 256.
        root = (root * 7) % 257;
      }
      break;
    
    case 1:
      // Receiver (ARM) prints out a sequence sent by the stress program 
      // Because the data to be re-transmitted does not necessarily fit in
      //  the cmdstr buffer, it is actually divided into lines, which we will
      //  successively read
      if (armsscanf(cmdstr, "%s %s %d %u", ui_cmdname, ui_strarg, 
        &stress_type, &data_length) < 4)
      {
        data_length = 0;
      }

      // We will set up an alternate IO handler to avoid blocking
      if (data_length > 0)
        ui_set_handler(ui_stress_handler1);

      // Set up the IO handler to wait for data_length lines and time out
      //  after one second
      ui_stress_data[0] = data_length;
      ui_stress_data[1] = UI_STRESS_HANDLER1_TIMEOUT;
      ui_stress_data[2] = stress_type;
      break;
    
    case 2: // PRINTF test
      // Stress program sends a formatting string, which is formatted by 
      //  the ARM with predefined integers. The ARM then sends it back.
      if (armsscanf(cmdstr, "%s %s %d %u", ui_cmdname, ui_strarg, 
        &stress_type, &data_length) < 4)
      {
        data_length = 0;
      }

      // We will set up an alternate IO handler to avoid blocking
      if (data_length > 0)
        ui_set_handler(ui_stress_handler1);

      // Set up the IO handler to wait for data_length lines and time out
      //  after one second
      ui_stress_data[0] = data_length;
      ui_stress_data[1] = UI_STRESS_HANDLER1_TIMEOUT;
      ui_stress_data[2] = stress_type;
      break;
  }
}

extern int swi_fail_count;

/** Tests something, specified by the first argument. E.g.
  * test ramfunc
  */
void ui_test (char * cmdstr)
{
  armprintf ("Disabled.");
  /*
  if (armsscanf(cmdstr, "%s %s", ui_cmdname, ui_strarg) < 2)
  {
    armprintf ("Test what?\r");
    return;
  }

  if (strncmp (ui_strarg, "ramfunc", sizeof(ui_strarg)) == 0)
  {
    armprintf ("Test RAM function at address %x\r", (int)ui_test_ramfunc);
    ui_test_ramfunc(cmdstr);
  }
  else if (strncmp (ui_strarg, "int", sizeof(ui_strarg)) == 0)
  {
    armprintf ("Testing interrupts.\r");
    ui_test_int(cmdstr);
  }
  else if (strncmp (ui_strarg, "stress", sizeof(ui_strarg)) == 0)
  {
    ui_test_stress(cmdstr);
  }
  else if (strncmp (ui_strarg, "bad_disable", sizeof(ui_strarg)) == 0)
  {
    armprintf ("Number of retarded calls to crit_disable_int: %d\r");
  }
  else
    armprintf ("Invalid argument(s) to %s.\r", "test");
  */
}

void ui_error ( char * cmdstr) 
{
  if (armsscanf(cmdstr, "%s %s", ui_cmdname, ui_strarg) < 2){
    armprintf (INVALID_ARGNUM_STR, "error");
    return;
  }

  if (strncmp (ui_strarg, "clear", sizeof(ui_strarg)) == 0){
    error_clear(0xFFFFFFFF);
    armprintf ("Error flags cleared.\r");
    return;
  }
}

void ui_motor ( char * cmdstr)
{
  int motor, speed, last; 

  motor = 0;
  speed = 0;
  
  if (armsscanf(cmdstr, "%s %d %d %d", ui_cmdname, &motor, &speed, &last) < 3)
  {
    armprintf (INVALID_ARGNUM_STR, "motor");
    return;
  }
  if (armsscanf(cmdstr, "%s %d %d %d", ui_cmdname, &motor, &speed, &last) < 4)
  {
    motor_set_speed(motor, speed);
    return;
  }
  motor_set_speed_slew(motor, speed, last);
}

void ui_mi ( char * cmdstr)
{

  if (armsscanf(cmdstr, "%s %s", ui_cmdname, ui_strarg) < 2)
    return;

  if(strncmp (ui_strarg, "on", sizeof(ui_strarg)) == 0)
    mi_start();
}

void ui_recharge (char * cmdstr) {
  if (armsscanf(cmdstr, "%s %s", ui_cmdname, ui_strarg) < 2)
    return;

  // If given 'start' as an argument, initialize the charger finder and start
  //  it
  if (strncmp (ui_strarg, "start", sizeof(ui_strarg)) == 0) {
    recharger_reset();
    recharger_enable();
  }
  else if (strncmp (ui_strarg, "stop", sizeof(ui_strarg)) == 0) {
    recharger_disable();
  }
}

void ui_avr (char * cmdstr) {
  int command;

  if (armsscanf (cmdstr, "%s %d", ui_cmdname, &command) < 2)
    return;

  if (command) motor_enable_charging();
  else motor_disable_charging();
}

void ui_pid ( char * cmdstr)
{
  unsigned int pid, i;
  
  if (armsscanf(cmdstr, "%s %s %u", ui_cmdname, ui_strarg, &pid) < 3)
  {
    if (strncmp (ui_strarg, "stat", sizeof(ui_strarg)) == 0){
      for(i = 0; i <= EVENT_MAX; i++)
        armprintf ("pid %d status: %s\r", i,
          (event_flags & (1<<i)) ? "OK" : "STOPPED");
      return;
    }
    armprintf ("pid [start|stop|stat] #\r");
    return;
  } 

  if (strncmp (ui_strarg, "start", sizeof(ui_strarg)) == 0)
  {
    event_start(pid);
    return;
  }

  if (strncmp (ui_strarg, "init", sizeof(ui_strarg)) == 0)
  {
    event_init(pid);
    return;
  }
  
  if (strncmp (ui_strarg, "stop", sizeof(ui_strarg)) == 0)
  {
    event_stop(pid);
    return;
  }

  if (strncmp (ui_strarg, "stat", sizeof(ui_strarg)) == 0){
    armprintf ("pid %d status: %s", pid,
      (event_flags & (1<<pid)) ? "OK" : "STOPPED");
    return;
  }
  armprintf ("pid [start|stop|stat] #\r");
  
}

/* int ui_random()
{
  return (AT91C_BASE_TC0->TC_CV);
} */
