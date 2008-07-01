#ifndef USER_INTERFACE_H 
#define USER_INTERFACE_H

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

#endif /* USER_INTERFACE_H */
