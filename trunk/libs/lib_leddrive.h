#include "lib_ledctl.h"

#ifndef LIB_LEDDRIVE_H
#define LIB_LEDDRIVE_H

//RGB LED array of structures
struct rgbled {
	unsigned char r;
	unsigned char g;
	unsigned char b;
}LED[16];
//States of leddrive_event()
enum leddrive_states {STARTUP,BATSTATUS,ANGLE,ROTATE,GRADIENT,CLEAR,STOP};
//possible gradients for cval
enum leddrive_gradient {BLACKWHITE,STOPLIGHT,BLUERED};

unsigned int leddrive_state;

//varibles for external functions.
int *leddrive_rot;
int leddrive_startver;
unsigned int leddrive_grad1;
unsigned int leddrive_grad2;
unsigned int leddrive_batlvl;//
unsigned int *leddrive_angledeg;
unsigned int *leddrive_anglecval;
unsigned int *leddrive_gradcval1;
unsigned int *leddrive_gradcval2;


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
*/
void anglelight(int angle, unsigned char r,unsigned char g,unsigned char b);
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
void leddrive_angle(unsigned int *deg,unsigned int *cval,unsigned int grad);
void leddrive_rotate(int *rot);
void leddrive_clear(void);//blanks led's
void leddrive_stop(void);//keeps current color states on led's
void leddrive_gradient(unsigned int *cval1,unsigned int *cval2,unsigned int grad1,unsigned int grad2);



/*
cval ,for now, is 0-4095, 0 being blue 4095 being red, with numbers between a liner gradient between the two.
*/

void leddrive_event(void);//MAIN EVEN CONTROLLER
void leddrive_init(void);
void leddrive_write(void);//sends LED[] values to the leds

#endif /* LIB_LEDDRIVE_H */
