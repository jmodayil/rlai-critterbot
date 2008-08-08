
#ifndef LIB_LEDDRIVE_H
#define LIB_LEDDRIVE_H

#include "lib_ledctl.h"
#include "lib_events.h"
#include <stdlib.h>


//cos/sin of 0-359 degress multiplied by 9000
int cos_lookup[360] = {9000,8999,8995,8988,8978,8966,8951,8933,8912,8889,8863,8835,8803,8769,8733,8693,8651,8607,8560,8510,8457,8402,8345,8285,8222,8157,8089,8019,7947,7872,7794,7715,7632,7548,7461,7372,7281,7188,7092,6994,6894,6792,6688,6582,6474,6364,6252,6138,6022,5905,5785,5664,5541,5416,5290,5162,5033,4902,4769,4635,4500,4363,4225,4086,3945,3804,3661,3517,3371,3225,3078,2930,2781,2631,2481,2329,2177,2025,1871,1717,1563,1408,1253,1097,941,784,628,471,314,157,-0,-157,-314,-471,-628,-784,-941,-1097,-1253,-1408,-1563,-1717,-1871,-2025,-2177,-2329,-2481,-2631,-2781,-2930,-3078,-3225,-3371,-3517,-3661,-3804,-3945,-4086,-4225,-4363,-4500,-4635,-4769,-4902,-5033,-5162,-5290,-5416,-5541,-5664,-5785,-5905,-6022,-6138,-6252,-6364,-6474,-6582,-6688,-6792,-6894,-6994,-7092,-7188,-7281,-7372,-7461,-7548,-7632,-7715,-7794,-7872,-7947,-8019,-8089,-8157,-8222,-8285,-8345,-8402,-8457,-8510,-8560,-8607,-8651,-8693,-8733,-8769,-8803,-8835,-8863,-8889,-8912,-8933,-8951,-8966,-8978,-8988,-8995,-8999,-9000,-8999,-8995,-8988,-8978,-8966,-8951,-8933,-8912,-8889,-8863,-8835,-8803,-8769,-8733,-8693,-8651,-8607,-8560,-8510,-8457,-8402,-8345,-8285,-8222,-8157,-8089,-8019,-7947,-7872,-7794,-7715,-7632,-7548,-7461,-7372,-7281,-7188,-7092,-6994,-6894,-6792,-6688,-6582,-6474,-6364,-6252,-6138,-6022,-5905,-5785,-5664,-5541,-5416,-5290,-5162,-5033,-4902,-4769,-4635,-4500,-4363,-4225,-4086,-3945,-3804,-3661,-3517,-3371,-3225,-3078,-2930,-2781,-2631,-2481,-2329,-2177,-2025,-1871,-1717,-1563,-1408,-1253,-1097,-941,-784,-628,-471,-314,-157,0,157,314,471,628,784,941,1097,1253,1408,1563,1717,1871,2025,2177,2329,2481,2631,2781,2930,3078,3225,3371,3517,3661,3804,3945,4086,4225,4363,4500,4635,4769,4902,5033,5162,5290,5416,5541,5664,5785,5905,6022,6138,6252,6364,6474,6582,6688,6792,6894,6994,7092,7188,7281,7372,7461,7548,7632,7715,7794,7872,7947,8019,8089,8157,8222,8285,8345,8402,8457,8510,8560,8607,8651,8693,8733,8769,8803,8835,8863,8889,8912,8933,8951,8966,8978,8988,8995,8999};
int sin_lookup[360] = {0,157,314,471,628,784,941,1097,1253,1408,1563,1717,1871,2025,2177,2329,2481,2631,2781,2930,3078,3225,3371,3517,3661,3804,3945,4086,4225,4363,4500,4635,4769,4902,5033,5162,5290,5416,5541,5664,5785,5905,6022,6138,6252,6364,6474,6582,6688,6792,6894,6994,7092,7188,7281,7372,7461,7548,7632,7715,7794,7872,7947,8019,8089,8157,8222,8285,8345,8402,8457,8510,8560,8607,8651,8693,8733,8769,8803,8835,8863,8889,8912,8933,8951,8966,8978,8988,8995,8999,9000,8999,8995,8988,8978,8966,8951,8933,8912,8889,8863,8835,8803,8769,8733,8693,8651,8607,8560,8510,8457,8402,8345,8285,8222,8157,8089,8019,7947,7872,7794,7715,7632,7548,7461,7372,7281,7188,7092,6994,6894,6792,6688,6582,6474,6364,6252,6138,6022,5905,5785,5664,5541,5416,5290,5162,5033,4902,4769,4635,4500,4363,4225,4086,3945,3804,3661,3517,3371,3225,3078,2930,2781,2631,2481,2329,2177,2025,1871,1717,1563,1408,1253,1097,941,784,628,471,314,157,-0,-157,-314,-471,-628,-784,-941,-1097,-1253,-1408,-1563,-1717,-1871,-2025,-2177,-2329,-2481,-2631,-2781,-2930,-3078,-3225,-3371,-3517,-3661,-3804,-3945,-4086,-4225,-4363,-4500,-4635,-4769,-4902,-5033,-5162,-5290,-5416,-5541,-5664,-5785,-5905,-6022,-6138,-6252,-6364,-6474,-6582,-6688,-6792,-6894,-6994,-7092,-7188,-7281,-7372,-7461,-7548,-7632,-7715,-7794,-7872,-7947,-8019,-8089,-8157,-8222,-8285,-8345,-8402,-8457,-8510,-8560,-8607,-8651,-8693,-8733,-8769,-8803,-8835,-8863,-8889,-8912,-8933,-8951,-8966,-8978,-8988,-8995,-8999,-9000,-8999,-8995,-8988,-8978,-8966,-8951,-8933,-8912,-8889,-8863,-8835,-8803,-8769,-8733,-8693,-8651,-8607,-8560,-8510,-8457,-8402,-8345,-8285,-8222,-8157,-8089,-8019,-7947,-7872,-7794,-7715,-7632,-7548,-7461,-7372,-7281,-7188,-7092,-6994,-6894,-6792,-6688,-6582,-6474,-6364,-6252,-6138,-6022,-5905,-5785,-5664,-5541,-5416,-5290,-5162,-5033,-4902,-4769,-4635,-4500,-4363,-4225,-4086,-3945,-3804,-3661,-3517,-3371,-3225,-3078,-2930,-2781,-2631,-2481,-2329,-2177,-2025,-1871,-1717,-1563,-1408,-1253,-1097,-941,-784,-628,-471,-314,-157};

//RGB LED array of structures
struct rgbled {
	unsigned char r;
	unsigned char g;
	unsigned char b;
}LED[16];
//angle tracking info
struct angleinfo {
	unsigned int grad;
	unsigned int *cval;//a cval that is >4096 is the trigger to not display the angle
	int *deg;
}ANGLEINFO[4];

//States of leddrive_event()
enum leddrive_states {STARTUP,BATSTATUS,ANGLE,ROTATE,GRADIENT,CLEAR,STOP,BALL,FADEANGLE,ERROR,EMERG,BUSY,COLORDIS};
//possible gradients for cval
enum leddrive_gradient {BLACKWHITE,STOPLIGHT,BLUERED,RED,GREEN,BLUE};

unsigned int leddrive_state;

//variables for ball simulation event
unsigned int ledball_angle;
unsigned int ledball_cval;
//ledball simulator controller
void ledball_crtl(void);


//varibles for external functions.
int *leddrive_rot;
int leddrive_startver;
unsigned int leddrive_batlvl;//
unsigned int leddrive_grad1;
unsigned int leddrive_grad2;
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
fade angle rgb values are displayed on neigbouring leds with diminished strenght, and blends with other colors.
*/
void anglelight(int angle, unsigned char r,unsigned char g,unsigned char b);
void fadeangle(int angle, unsigned char r,unsigned char g,unsigned char b);
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
void leddrive_error(void);
void leddrive_emerg(void);
void leddrive_colordisplay(void);
void leddrive_busy(void);
/*
cval is 0-4095, leddrive_grad1/2 decide what cval will represent.
*/

int leddrive_event(void);//MAIN EVEN CONTROLLER
int leddrive_init(void);
void leddrive_write(void);//sends LED[] values to the leds

#endif /* LIB_LEDDRIVE_H */
