/*
 *  lib_ledctl.h
 *  
 *
 *  Created by Marc G. Bellemare on 08/04/08
 *  Last Revision: 30/06/08
 *
 */

#ifndef LIB_LEDCTL_H
#define LIB_LEDCTL_H

// Defines which LED controller deals which each color channel
#define BLUE_CONTROLLER  0
#define GREEN_CONTROLLER 1
#define RED_CONTROLLER   2

// Number of LEDs per controller
#define LEDCTL_NUM_LEDS 16

// Number of controllers
#define LEDCTL_NUM_CONTROLLERS 3

// Maximum LED values - arbitrarily set at 4096 for now
#define LEDCTL_MAX_VALUE 4096 

// Maxium Dot Correction value
#define LEDCTL_DC_MAX_VALUE 64

// Define word sizes for dot correction and grayscale modes
//  Value is word_size - 1
#define LEDCTL_DC_WORD_SIZE 5
// GS_WORD_SIZE must correspond to SSC_WORD_SIZE!
#define LEDCTL_GS_WORD_SIZE 11

// Various state that the LED controller can be in
enum ledctl_state_type { 
  GRAYSCALE, FIRST_GRAYSCALE, 
  DOT_CORRECTION, FIRST_DOT_CORRECTION };


// The 100Hz event function for the LED driver
void ledctl_event();

// Send all data
void ledctl_senddata_all(); 
// Send all DC data
void ledctl_senddata_dc(); 

/** Applies initial dot correction. This function waits until the DC data 
  * has been sent before returning and should not generally be used, apart
  * from during the initialization of the LED driver.
  */
void ledctl_dc( void );

/** Initialize the LED controller driver. This should be called
  * before the SPI or the 100Hz timer are started, to avoid
  * damaging the LEDs.
  */
void ledctl_init ();
/** Will be made obsolete - runs the 100Hz timer. */
void ledctl_inittimer ();

/** Returns whether the grayscale data has been sent; this information is
    only useful when the caller has actually requested data to be sent. */
unsigned int ledctl_data_sent();

/** Changes mode. GRAYSCALE and DOT_CORRECTION are mapped to FIRST_GRAYSCALE
  * and FIRST_DOT_CORRECTION, respectively. */
void ledctl_setmode(enum ledctl_state_type mode);

/** Interface to the LED controller driver
  *  Set an LED value
  *
  *  device must be in range 0-2
  *  led must be in range 0-15
  *  value must be in the range 0-4095
  */
void ledctl_setvalue(int device, int led, int value);

/** Interface to the LED controller driver
  *  Retrieve a LED value from the driver arrays 
  *
  *  device must be in range 0-2
  *  led must be in range 0-15
  *  value must be in the range 0-4095
  */
int ledctl_getvalue(int device, int led);

/** Interface to the LED controller driver
  *  Returns the status of a LED, as per the LED controller
  *
  *  device must be in range 0-2
  *  led must be in range 0-15
  *  value must be in the range 0-4095
  */
int ledctl_getstatus(int device, int led);

/** Interface to the LED controller driver
  *  Sets the color of a LED
  *
  *  led must be in range 0-15
  *  red, green, blue must be in the range 0-4095
  */
void ledctl_setcolor(int led, int red, int green, int blue);

/** Interface to the LED controller driver
  *  Returns one of the three color values of a LED
  *
  *  led must be in range 0-15
  *  color must be in range 0-2
  */
int ledctl_getcolor(int led, int color);

/** Interface to the LED controller driver
  *  Sets the DC values for a LED
  *
  *  led must be in range 0-15
  *  red, green, blue must be in the range 0-63
  */
void ledctl_setdc(int led, int red, int green, int blue);

/** Interface to the LED controller driver
  *  Returns one of the three DC values for a LED
  *
  *  led must be in range 0-15
  *  color must be in range 0-2
  */
int ledctl_getdc(int led, int color);

/** Interface to the LED controller driver
  *  Returns 16 bits, one per LED, containing LOD (LED Open Detection)
  *   status. A set bit indicates that the LED is open.
  *
  *  color must be in range 0-2
  */
unsigned short ledctl_getLOD (int color);

/** Interface to the LED controller driver
  *  Returns nonzero if the TEF (Thermal Error Flag) of one of the LED 
  *   controllers is high. Bit 0-2 are set for each particular TEF.
  *
  */
unsigned int ledctl_getTEF();

/** Interface to the LED controller driver
  *  Returns nonzero if XERR is low, i.e. an error has been detected by
  *   the LED controllers. Read LOD and TEF to get more detailed information.
  *
  */
unsigned int ledctl_geterr();
#endif /* LIB_LEDCTL_H */
