/*
 *  lib_ui.h
 *  
 *
 *  Created by Marc G. Bellemare on 27/06/08.
 *  Last Revision: 1/07/08
 *
 */

#ifndef LIB_UI_H
#define LIB_UI_H

// How frequently to report status in reporting mode, in 100th of a second
#define UI_REPORT_INTERVAL 200

typedef void (*ui_function) (char * cmdstr);

/** Commands available in the user interface. */

/** User Interface: set LED command
  *  Called in response to set_led
  *
  * argc is the number of arguments
  * argStart is the first argument
  * (These parameters may change soon)
  */
void ui_setled (char * cmdstr);
void ui_getled (char * cmdstr);
void ui_statled (char * cmdstr);
void ui_help (char * cmdstr);
void ui_fortune (char * cmdstr);
void ui_test (char * cmdstr);
void ui_setdot (char * cmdstr);
void ui_getdot (char * cmdstr);
void ui_getaccel (char * cmdstr);
void ui_status (char * cmdstr);
void ui_report (char * cmdstr);
void ui_mode (char * cmdstr);

void ui_clearall (char * cmdstr);
void ui_setall (char * cmdstr);

/** Main User Interface function.
  * Should be called at regular intervals.
  *
  * Does a non-blocking check for input, which it immediately processes.
  */
void ui_event();

/** Reports general status and information on the serial port.
  * Don't rely on this function to produce structured output.
  */
void ui_do_report();

#endif /* LIB_UI_H */
