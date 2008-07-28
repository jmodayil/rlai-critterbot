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

// Included for EOF, NULL
#include <stdio.h>

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
  {"status", ui_status, "status"},
  {"report", ui_report, "report"},
  {"mode", ui_mode, "mode <led [gs|dc]>"},
  {"test", ui_test, "test [ramfunc|sanity]"},
  {"bootloader", ui_bootloader, "bootloader - do not use"},
  {"reset", ui_reset, "reset"},
  {"fortune", ui_fortune, "fortune"}
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

void ui_event()
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
    return;
  // Parse command, return false if not found
  ui_cmd_item * cmd = ui_parse_command(ui_command_string);
  // Not found, write error message
  if (cmd == NULL)
  {
    armprintf ("Invalid command: %s\n", ui_command_string);
    return;
  }
  else
  {
    // Call function
    (*(cmd->func))(ui_command_string); 
  }
}

void ui_do_report()
{
  // The lazy way - call other status-related functions from the UI
  ui_statled("stat_led");
  ui_getaccel("get_accel");
  armprintf ("Error status: %x\n", error_get());
}

void ui_help(char * cmdstr)
{
  int idx;
  
  armprintf ("List of commands and usage: \n");

  for (idx = 0; idx < ui_ncommands; idx++)
    armprintf ("%s - %s\n", ui_commands[idx].name, 
      ui_commands[idx].description);
  
  armprintf ("\nNote! Commands and arguments are case-sensitive.\n");
}

void ui_setled(char * cmdstr)
{
  int ledNum;
  int ledColors[3];

  // parse in num, color
  if (armsscanf (cmdstr, "%s %d %d %d %d", ui_cmdname,
    &ledNum, &ledColors[0], &ledColors[1], &ledColors[2]) < 5)
  {
    armprintf ("Invalid number of arguments to set_led.\n");
    return;
  }

  if (ledNum < 0 || ledNum >= LEDCTL_NUM_LEDS ||
      ledColors[0] < 0 || ledColors[1] < 0 || ledColors[2] < 0 ||
      ledColors[0] >= LEDCTL_MAX_VALUE || 
      ledColors[1] >= LEDCTL_MAX_VALUE ||
      ledColors[2] >= LEDCTL_MAX_VALUE)
  {
    armprintf ("LED must be in range 0..%d\n", LEDCTL_NUM_LEDS);
    armprintf ("Colors must be in range 0..%d\n", LEDCTL_MAX_VALUE);
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
    armprintf ("Invalid number of arguments to get_led.\n");
    return;
  }

  if (ledNum < 0 || ledNum >= LEDCTL_NUM_LEDS)
  {
    armprintf ("LED must be in range 0..%d\n", LEDCTL_NUM_LEDS);
    return;
  }
  
  for (color = 0; color < 3; color++)
    ledColors[color] = ledctl_getcolor(ledNum, color);

  armprintf ("LED %d: %d,%d,%d\n", ledNum, 
    ledColors[0], ledColors[1], ledColors[2]);
}

void ui_statled(char * cmdstr)
{
  int i;

  armprintf ("XERR status: %d\n", ledctl_geterr());
  for (i = 0; i < LEDCTL_NUM_CONTROLLERS; i++)
    armprintf ("LOD %d status: %x\n", i, ledctl_getLOD(i));
  armprintf ("TEF status: %x\n", ledctl_getTEF());

  armprintf ("If LOD or TEF != 0 and XERR is on or vice-versa, "
    "report to MGB.\n");
  armprintf ("\n");
}

void ui_status(char * cmdstr)
{
  armprintf ("LED status: %s\n", STATUS_STRING(!ledctl_geterr()));
  armprintf ("Accelerometer status: %s\n", "N/A");
  armprintf ("Error status: %x\n", error_get());

  armprintf ("Coffee consumption: 0.266 cup / hr\n");
  armprintf("\n");
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
    armprintf ("Invalid number of arguments to setall.\n");
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
    armprintf ("Invalid number of arguments to set_dot.\n");
    return;
  }

  if (ledNum < 0 || ledNum >= LEDCTL_NUM_LEDS ||
      ledColors[0] < 0 || ledColors[1] < 0 || ledColors[2] < 0 ||
      ledColors[0] >= LEDCTL_DC_MAX_VALUE || 
      ledColors[1] >= LEDCTL_DC_MAX_VALUE ||
      ledColors[2] >= LEDCTL_DC_MAX_VALUE)
  {
    armprintf ("LED must be in range 0..%d\n", LEDCTL_NUM_LEDS);
    armprintf ("Colors must be in range 0..%d\n", LEDCTL_MAX_VALUE);
    return;
  }

  // Set the LED's three colors
  ledctl_setdc(ledNum, ledColors[0], ledColors[1], ledColors[2]);

  // Re-apply dot-correction right away
  armprintf ("Applying dot correction...");
  // ledctl_dc();
  armprintf ("Not yet! Use mode led dc.\n");
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
    armprintf ("LED must be in range 0..%d\n", LEDCTL_NUM_LEDS);
    return;
  }

  while (ledNum < LEDCTL_NUM_LEDS)
  {
    for (color = 0; color < 3; color++)
      ledColors[color] = ledctl_getdc(ledNum, color);
    armprintf ("LED %d dot correction: %d,%d,%d\n", ledNum, 
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
  armprintf ("\n");
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
    armprintf ("Invalid argument(s) to ui_mode.\n");
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
      armprintf ("Valid LED modes are: gs, dc.\n");
  }
  else
    armprintf ("Valid modes are: led.\n");
}


void ui_bootloader(char * cmdstr)
{
  unsigned int data_size;

  // As 'status' report, tell the user if the bootloader core is not in RAM
  if (((char *)boot_core) < AT91C_ISRAM)
  {
    armprintf ("boot_core not in RAM\n");
    error_set (ERR_BOOT);
    return;
  }

  if (armsscanf(cmdstr, "%s %s %d", ui_cmdname, ui_strarg, &data_size) < 3)
  {
    armprintf ("This command is for bootloader purposes. "
      "Insert disk #127 to continue (or refer to the manual).\n");
    return;
  }
  
  // Test for password, parse data size
  if (strncmp(ui_strarg, BOOT_PASSWORD, sizeof(ui_strarg)) != 0)
  {
    armprintf ("Wrong password.\n");
    error_set (ERR_BOOT);
    return;
  }

  // Unlikely data size or too big; we could make 0 a threshold (e.g. <= 500)
  if (data_size <= 0 || data_size >= BOOT_MAX_CODE_SIZE)
  {
    armprintf ("Data size is wrong: %d\n", data_size);
    error_set(ERR_BOOT);
    return;
  }

  // Call the no-return function
  boot_begin_receive(data_size);
}

void ui_reset(char * cmdstr)
{
  if (armsscanf(cmdstr, "%s %s", ui_cmdname, ui_strarg) < 2)
  {
    armprintf ("Usage: reset %s\n", UI_RESET_PASSWORD);
    return;
  }
 
  if (strncmp(ui_strarg, UI_RESET_PASSWORD, sizeof(ui_strarg)) != 0)
  {
    armprintf ("Wrong password (try %s).\n", UI_RESET_PASSWORD);
    return;
  }

  boot_reset_arm();
}

RAMFUNC void ui_test_ramfunc(char * cmdstr)
{
  armprintf (cmdstr);
  armprintf ("\n");
}

/** Tests something, specified by the first argument. E.g.
  * test ramfunc
  */
void ui_test (char * cmdstr)
{
  if (armsscanf(cmdstr, "%s %s", ui_cmdname, ui_strarg) < 2)
  {
    armprintf ("Test what?\n");
    return;
  }

  if (strncmp (ui_strarg, "ramfunc", sizeof(ui_strarg)) == 0)
  {
    armprintf ("Test RAM function at address %x\n", (int)ui_test_ramfunc);
    ui_test_ramfunc(cmdstr);
  }
  else if (strncmp (ui_strarg, "sanity", sizeof(ui_strarg)) == 0)
    armprintf ("You rolled a 6. Sanity check failed.\n");
  else
    armprintf ("Invalid argument.\n");
}

static int fortune_idx = 0;

char * fortunes[] = {
  "It usually takes more than three weeks to prepare a good impromptu speech. (Mark Twain)",
  "Qui vole un oeuf vole un boeuf",
  "Father, I want to kill you. Mother, I want to oooo-doo-dii",
  "You should be working",
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
  armprintf ("\n");
}

