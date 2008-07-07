/*
 *  lib_ui.c
 *  
 *
 *  Created by Marc G. Bellemare on 27/06/08.
 *  Last Revision: 1/07/08
 *
 */

#include "compiler.h"
#include "lib_ui.h"
#include "lib_ledctl.h"

// Included for EOF, NULL
#include <stdio.h>

/* Structure used to hold command name + function pairs */
typedef struct {
  char * name;
  ui_function func;
  char * description;
} ui_cmd_item;

void ui_fortune(char * cmdstr);

ui_cmd_item ui_commands[] = {
  { "help", ui_help, "Do you need help?"}, // help
  { "set_led", ui_setled, "set_led <led #> <red> <green> <blue>"}, 
    // set_led <led num, 0-15> <red> <green> <blue>
  { "get_led", ui_getled, "get_led <led #>"},
    // get_led <led_num, 0-15>
  {"stat_led", ui_statled, "stat_led"},
  {"fortune", ui_fortune, "fortune"}
};

int ui_ncommands = sizeof(ui_commands)/sizeof(*ui_commands);

char ui_command_string[256];
char ui_cmdname[64];

/** Finds the requested command in our list of commands.
  * Returns the corresponding item if found, NULL otherwise.
  */

ui_cmd_item * ui_parse_command(char * cmdstr)
{
  int idx;

  armsscanf(cmdstr, "%s", ui_cmdname);

  for (idx = 0; idx < ui_ncommands; idx++)
    if (strncmp(ui_commands[idx].name, ui_cmdname, sizeof(ui_cmdname)) == 0)
      return &ui_commands[idx];

  return NULL;
}

void ui_event()
{
  // Check wthether we have new data
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

void ui_help(char * cmdstr)
{
  int idx;
  
  armprintf ("List of commands: \n");

  for (idx = 0; idx < ui_ncommands; idx++)
    armprintf ("%s - %s\n", ui_commands[idx].name, 
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
  armprintf ("Aha! LED status not implemented!\n");
}



static int fortune_idx = 0;

char * fortunes[] = {
  "It usually takes more than three weeks to prepare a good impromptu speech. (Mark Twain)",
  "Qui vole un oeuf vole un boeuf",
  "Father, I want to kill you. Mother, I want to oooo-doo-dii",
  "You should be working",

  // Keep this 0 here!
  0
  };

/** Unecessary waste of work time */
void ui_fortune(char * cmdstr)
{
  armprintf (fortunes[fortune_idx]);
  fortune_idx++;
  if (fortunes[fortune_idx] == 0)
    fortune_idx = 0;
}

