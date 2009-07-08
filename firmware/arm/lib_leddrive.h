
#ifndef LIB_LEDDRIVE_H
#define LIB_LEDDRIVE_H

#include "lib_ledctl.h"
#include "lib_events.h"
#include <stdlib.h>

#define LED_NUM_LEDS 16

//RGB LED array of structures
struct rgbled {
	unsigned char r;
	unsigned char g;
	unsigned char b;
}LED[LED_NUM_LEDS];
//angle tracking info
struct angleinfo {
	unsigned int grad;
	unsigned int *cval;//a cval that is >4096 is the trigger to not display the angle
	int *deg;
}ANGLEINFO[4];

//States of leddrive_event()
enum leddrive_states {STARTUP,CHARGESTATUS,BATSTATUS,ANGLE,ROTATE,GRADIENT,CLEAR,STOP,BALL,FADEANGLE,ERROR,EMERG,BUSY,COLORDIS,BYTE,RAINBOW,CUSTOM};
//possible gradients for cval
enum leddrive_gradient {BLACKWHITE,STOPLIGHT,BLUERED,RED,GREEN,BLUE};

unsigned int leddrive_state;

//variables for ball simulation event
unsigned int ledball_angle;
unsigned int ledball_cval;
//ledball simulator controller
void ledball_crtl(void);
//charge max indicator
unsigned int leddrive_charge_max;

//varibles for external functions.
int *leddrive_rot;
int leddrive_startver;
unsigned int leddrive_batlvl;//
unsigned int leddrive_grad1;
unsigned int leddrive_grad2;
unsigned int *leddrive_gradcval1;
unsigned int *leddrive_gradcval2;

void dispbyte(unsigned char val);

//internal functions
/*
*light points to a single r,g,or b value 0-255 and increase/decreases untill ending.
*/
void fadein(unsigned char *light,unsigned char incr,unsigned char ending);
void fadeout(unsigned char *light,unsigned char incr,unsigned char ending);
/*
*fades the led to the rgb values at a rate of incr.
*/
void fadeto(struct rgbled *light, unsigned char r,unsigned char g,unsigned char b,unsigned char incr);
/*
pulses led from black to rgb values fading in at a rate of incr takeing lenght cycles before fading back to black
*/
void pulse(struct rgbled *light,unsigned char r,unsigned char g,unsigned char b,unsigned char incr,unsigned int length);
/*
takes angle and chooses a corresponding led and displays rgb color on that led. angle values will wrap around
fade angle rgb values are displayed on neigbouring leds with diminished strenght, and blends with other colors.
*/
void anglelight(int angle, unsigned char r,unsigned char g,unsigned char b);
void fadeangle(int angle, unsigned char r,unsigned char g,unsigned char b);
/*
 * Display a rainbow pattern for good looks and cheer.
 */
void rainbow(void);
// Calculate how full our charge is right now
void led_chargemax(int max);

/*
displays the battery lvl 0-100
*/
void battlvl(unsigned int lvl);
/*
takes 2 rgb values and makes a gradient from one to the other.
*/
void gradient(unsigned char r1,unsigned char g1,unsigned char b1,unsigned char r2,unsigned char g2,unsigned char b2);
/*
rotates all led's. positive number clockwise and visvera. Rot is the number of cycles to wait before rotating again.
*/
void rotate(int rot);
/*
startup sequence using varible leddrive_startver to pick which sequence to use
*/
void startup(void);

void clearled(void);

//takes cval and a gradient to use to provide RGB values.
void cvalselect(unsigned char *r,unsigned char *g,unsigned char *b,unsigned int grad, unsigned int cval);

//external functions, State changers
/*
All change leddrive_event's state accordingly.
*/
void leddrive_startup(int ver);
void leddrive_batstatus(void);
void leddrive_angle(unsigned int id,unsigned int grad,unsigned int *cval,int *deg);
void leddrive_fadeangle(unsigned int id,unsigned int grad,unsigned int *cval,int *deg);
void leddrive_rotate(int *rot);
void leddrive_clear(void);//blanks led's
void leddrive_stop(void);//keeps current color states on led's
void leddrive_gradient(unsigned int *cval1,unsigned int *cval2,unsigned int grad1,unsigned int grad2);
void leddrive_ball(void);
void leddrive_chargestatus(void);
void leddrive_error(void);
void leddrive_emerg(void);
void leddrive_colordisplay(void);
void leddrive_busy(void);
void leddrive_byte(unsigned char *val);
void leddrive_rainbow(void);
void leddrive_custom(void);
/*
cval is 0-4095, leddrive_grad1/2 decide what cval will represent.
*/

int leddrive_event(void);//MAIN EVEN CONTROLLER
int leddrive_init(void);
void leddrive_write(void);//sends LED[] values to the leds

#endif /* LIB_LEDDRIVE_H */
