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
#include "lib_events.h"
#include "lib_adcspi.h"
#include "lib_adc.h"

// Included for EOF, NULL
#include <stdio.h>

event_s ui_event_s = {
  NULL,
  ui_event,
  0
};

#define STATUS_STRING(a) ( (a)? "OK" : "FAIL" )

/* Structure used to hold command name + function pairs */
typedef struct {
  char * name;
  ui_function func;
  char * description;
} ui_cmd_item;

extern struct ssc_packet ledctl_ssc_packet[];

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
  {"report", ui_report, "report"},
  {"mode", ui_mode, "mode <led [gs|dc]>"},
  {"test", ui_test, "test [ramfunc|int]"},
  {"bootloader", ui_bootloader, "bootloader - do not use"},
  {"reset", ui_reset, "reset"},
  {"fortune", ui_fortune, "fortune"},
  {"pid", ui_pid, "pid [start|stop|stat] #"},
  {"error", ui_error, "error [clear]"},
  {"motor", ui_motor, "motor [motor #] [speed #]"}
};

int ui_ncommands = sizeof(ui_commands)/sizeof(*ui_commands);

char ui_command_string[256];
char ui_cmdname[64];
char ui_strarg[64];

// Reporting mode - if nonzero, the UI will spit out some status information
//  every so often.
volatile int ui_report_mode = 0;
volatile int ui_report_clock = 0;

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

int ui_event()
{
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
  {
    armprintf ("Invalid command: \"%s\"\r", ui_command_string);
  }
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
  ui_statled("stat_led");
  ui_getaccel("get_accel");
  ui_getadcspi("get_adcspi");
  for(i = 4; i < 8; i++)
    armprintf("ADC Channel %d: %d\r", i, adc_output[i]);
  armprintf ("Error status: %x\r", error_get());
}

void ui_help(char * cmdstr)
{
  int idx;
  
  armprintf ("List of commands and usage: \r");

  for (idx = 0; idx < ui_ncommands; idx++)
    armprintf ("%s - %s\r", ui_commands[idx].name, 
      ui_commands[idx].description);
  
  armprintf ("\rNote! Commands and arguments are case-sensitive.\r");
}

void ui_setled(char * cmdstr)
{
  int ledNum;
  int ledColors[3];

  // parse in num, color
  if (armsscanf (cmdstr, "%s %d %d %d %d", ui_cmdname,
    &ledNum, &ledColors[0], &ledColors[1], &ledColors[2]) < 5)
  {
    armprintf ("Invalid number of arguments to set_led.\r");
    return;
  }

  if (ledNum < 0 || ledNum >= LEDCTL_NUM_LEDS ||
      ledColors[0] < 0 || ledColors[1] < 0 || ledColors[2] < 0 ||
      ledColors[0] >= LEDCTL_MAX_VALUE || 
      ledColors[1] >= LEDCTL_MAX_VALUE ||
      ledColors[2] >= LEDCTL_MAX_VALUE)
  {
    armprintf ("LED must be in range 0..%d\r", LEDCTL_NUM_LEDS);
    armprintf ("Colors must be in range 0..%d\r", LEDCTL_MAX_VALUE);
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
    armprintf ("Invalid number of arguments to get_led.\r");
    return;
  }

  if (ledNum < 0 || ledNum >= LEDCTL_NUM_LEDS)
  {
    armprintf ("LED must be in range 0..%d\r", LEDCTL_NUM_LEDS);
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
  armprintf ("LED status: %s\r", STATUS_STRING(!ledctl_geterr()));
  armprintf ("Accelerometer status: %s\r", "N/A");
  armprintf ("Error status: %x\r", error_get());

  armprintf ("Coffee consumption: 0.266 cup / hr\r");
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
    armprintf ("Invalid number of arguments to setall.\r");
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
    armprintf ("Invalid number of arguments to set_dot.\r");
    return;
  }

  if (ledNum < 0 || ledNum >= LEDCTL_NUM_LEDS ||
      ledColors[0] < 0 || ledColors[1] < 0 || ledColors[2] < 0 ||
      ledColors[0] >= LEDCTL_DC_MAX_VALUE || 
      ledColors[1] >= LEDCTL_DC_MAX_VALUE ||
      ledColors[2] >= LEDCTL_DC_MAX_VALUE)
  {
    armprintf ("LED must be in range 0..%d\r", LEDCTL_NUM_LEDS);
    armprintf ("Colors must be in range 0..%d\r", LEDCTL_MAX_VALUE);
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
    armprintf ("LED must be in range 0..%d\r", LEDCTL_NUM_LEDS);
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

  armprintf ("Accelerometer status and outputs: %d ", status);
  for (i = 0; i < ACCEL_NUM_AXES; i++)
    armprintf (" %d", accel_get_output(i));
  armprintf ("\r");
}

void ui_getadcspi (char * cmdstr)
{
  int i;
  int sel;
  int one_sel;

  armprintf ("Off-board ADC outputs: \r");
  // Print out one output if requested, otherwise all
  if (armsscanf(cmdstr, "%s %s", ui_cmdname, &i) >= 2)
  {
    sel = adcspi_is_selected(i);

    if (sel)
      armprintf ("%d: %d\r", i, adcspi_get_output(i));
    else
      armprintf ("%d: N/S\r", i);
  }
  else
  {
    one_sel = 0;

    // Loop throuh all outputs (64 of them) and print their value
    for (i = 0; i < ADCSPI_NUM_OUTPUTS; i++)
    {
      sel = adcspi_is_selected(i);

      if (sel)
      {
        armprintf ("%d (%d): %d\r", i, adcspi_get_address(i), 
          adcspi_get_output(i));
        one_sel |= 1;
      }
    }

    // If no selected outputs, print out 'No selected outputs'
    if (!one_sel)
      armprintf ("No selected outputs.\r");
  }
}

void ui_toggle_adcspi(char * cmdstr)
{
  int index;
  
  if (armsscanf(cmdstr, "%s %d", ui_cmdname, &index) < 2)
  {
    armprintf ("Invalid argument to adcspi_toggle.\r");
    return;
  }

  if (adcspi_is_selected(index))
    adcspi_deselect(index);
  else
    adcspi_select(index);
}

void ui_report (char * cmdstr)
{
  // Toggle reporting mode
  ui_report_mode ^= 0x1;
  if (!ui_report_mode)
    ui_report_clock = 0;
}

void ui_mode (char * cmdstr)
{
  char modestr[64];

  if (armsscanf(cmdstr, "%s %s %s", ui_cmdname, ui_strarg, modestr) < 3)
  {
    armprintf ("Invalid argument(s) to ui_mode.\r");
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
    armprintf ("This command is for bootloader purposes. "
      "Insert disk #127 to continue (or refer to the manual).\r");
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

  event_start(EVENT_ID_BOOT);
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

/** Tests something, specified by the first argument. E.g.
  * test ramfunc
  */
void ui_test (char * cmdstr)
{
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
  else
    armprintf ("Invalid argument.\r");
}

void ui_error ( char * cmdstr) 
{
  if (armsscanf(cmdstr, "%s %s", ui_cmdname, ui_strarg) < 2){
    armprintf ("error [clear]\r");
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
  int motor, speed;

  motor = 0;
  speed = 0;
  
  if (armsscanf(cmdstr, "%s %d %d", ui_cmdname, &motor, &speed) < 3)
  {
    armprintf ("Too few arguments");
    return;
  }

  motor_set_speed(motor, speed);
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

static int fortune_idx = 0;

char * fortunes[] = {
  "It usually takes more than three weeks to prepare a good impromptu speech. (Mark Twain)",
  "Qui vole un oeuf vole un boeuf",
  "Father, I want to kill you. Mother, I want to oooo-doo-dii",
  "Is it in RAM?",
  "Has it been powered on?",
  "Have you compiled?",
  "Have you reprogrammed the chip?",
  "Is it connected to the internet?",
  "Mini-golf is all about patience and correcting the putter angle",
  // Keep this 0 here!
  0
  };

int num_fortunes = -1;

/** Unecessary waste of work time */
void ui_fortune(char * cmdstr)
{
  int r = 0;

  if (num_fortunes == -1)
  {
    num_fortunes = 0;
    while (fortunes[fortune_idx] != 0)
    {
      fortune_idx++;
      num_fortunes++;
    }
  }

  // Random-number generate from TC0
  r = (AT91C_BASE_TC0->TC_CV % num_fortunes);

  armprintf (fortunes[r]);
  armprintf ("\r");
}

