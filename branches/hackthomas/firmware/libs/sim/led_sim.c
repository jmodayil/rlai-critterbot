#include <GL/gl.h>
#include <GL/glut.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define C 0.3925
//cos/sin of 0-359 degress multiplied by 9000
int cos_lookup[360] = {9000,8999,8995,8988,8978,8966,8951,8933,8912,8889,8863,8835,8803,8769,8733,8693,8651,8607,8560,8510,8457,8402,8345,8285,8222,8157,8089,8019,7947,7872,7794,7715,7632,7548,7461,7372,7281,7188,7092,6994,6894,6792,6688,6582,6474,6364,6252,6138,6022,5905,5785,5664,5541,5416,5290,5162,5033,4902,4769,4635,4500,4363,4225,4086,3945,3804,3661,3517,3371,3225,3078,2930,2781,2631,2481,2329,2177,2025,1871,1717,1563,1408,1253,1097,941,784,628,471,314,157,-0,-157,-314,-471,-628,-784,-941,-1097,-1253,-1408,-1563,-1717,-1871,-2025,-2177,-2329,-2481,-2631,-2781,-2930,-3078,-3225,-3371,-3517,-3661,-3804,-3945,-4086,-4225,-4363,-4500,-4635,-4769,-4902,-5033,-5162,-5290,-5416,-5541,-5664,-5785,-5905,-6022,-6138,-6252,-6364,-6474,-6582,-6688,-6792,-6894,-6994,-7092,-7188,-7281,-7372,-7461,-7548,-7632,-7715,-7794,-7872,-7947,-8019,-8089,-8157,-8222,-8285,-8345,-8402,-8457,-8510,-8560,-8607,-8651,-8693,-8733,-8769,-8803,-8835,-8863,-8889,-8912,-8933,-8951,-8966,-8978,-8988,-8995,-8999,-9000,-8999,-8995,-8988,-8978,-8966,-8951,-8933,-8912,-8889,-8863,-8835,-8803,-8769,-8733,-8693,-8651,-8607,-8560,-8510,-8457,-8402,-8345,-8285,-8222,-8157,-8089,-8019,-7947,-7872,-7794,-7715,-7632,-7548,-7461,-7372,-7281,-7188,-7092,-6994,-6894,-6792,-6688,-6582,-6474,-6364,-6252,-6138,-6022,-5905,-5785,-5664,-5541,-5416,-5290,-5162,-5033,-4902,-4769,-4635,-4500,-4363,-4225,-4086,-3945,-3804,-3661,-3517,-3371,-3225,-3078,-2930,-2781,-2631,-2481,-2329,-2177,-2025,-1871,-1717,-1563,-1408,-1253,-1097,-941,-784,-628,-471,-314,-157,0,157,314,471,628,784,941,1097,1253,1408,1563,1717,1871,2025,2177,2329,2481,2631,2781,2930,3078,3225,3371,3517,3661,3804,3945,4086,4225,4363,4500,4635,4769,4902,5033,5162,5290,5416,5541,5664,5785,5905,6022,6138,6252,6364,6474,6582,6688,6792,6894,6994,7092,7188,7281,7372,7461,7548,7632,7715,7794,7872,7947,8019,8089,8157,8222,8285,8345,8402,8457,8510,8560,8607,8651,8693,8733,8769,8803,8835,8863,8889,8912,8933,8951,8966,8978,8988,8995,8999};
int sin_lookup[360] = {0,157,314,471,628,784,941,1097,1253,1408,1563,1717,1871,2025,2177,2329,2481,2631,2781,2930,3078,3225,3371,3517,3661,3804,3945,4086,4225,4363,4500,4635,4769,4902,5033,5162,5290,5416,5541,5664,5785,5905,6022,6138,6252,6364,6474,6582,6688,6792,6894,6994,7092,7188,7281,7372,7461,7548,7632,7715,7794,7872,7947,8019,8089,8157,8222,8285,8345,8402,8457,8510,8560,8607,8651,8693,8733,8769,8803,8835,8863,8889,8912,8933,8951,8966,8978,8988,8995,8999,9000,8999,8995,8988,8978,8966,8951,8933,8912,8889,8863,8835,8803,8769,8733,8693,8651,8607,8560,8510,8457,8402,8345,8285,8222,8157,8089,8019,7947,7872,7794,7715,7632,7548,7461,7372,7281,7188,7092,6994,6894,6792,6688,6582,6474,6364,6252,6138,6022,5905,5785,5664,5541,5416,5290,5162,5033,4902,4769,4635,4500,4363,4225,4086,3945,3804,3661,3517,3371,3225,3078,2930,2781,2631,2481,2329,2177,2025,1871,1717,1563,1408,1253,1097,941,784,628,471,314,157,-0,-157,-314,-471,-628,-784,-941,-1097,-1253,-1408,-1563,-1717,-1871,-2025,-2177,-2329,-2481,-2631,-2781,-2930,-3078,-3225,-3371,-3517,-3661,-3804,-3945,-4086,-4225,-4363,-4500,-4635,-4769,-4902,-5033,-5162,-5290,-5416,-5541,-5664,-5785,-5905,-6022,-6138,-6252,-6364,-6474,-6582,-6688,-6792,-6894,-6994,-7092,-7188,-7281,-7372,-7461,-7548,-7632,-7715,-7794,-7872,-7947,-8019,-8089,-8157,-8222,-8285,-8345,-8402,-8457,-8510,-8560,-8607,-8651,-8693,-8733,-8769,-8803,-8835,-8863,-8889,-8912,-8933,-8951,-8966,-8978,-8988,-8995,-8999,-9000,-8999,-8995,-8988,-8978,-8966,-8951,-8933,-8912,-8889,-8863,-8835,-8803,-8769,-8733,-8693,-8651,-8607,-8560,-8510,-8457,-8402,-8345,-8285,-8222,-8157,-8089,-8019,-7947,-7872,-7794,-7715,-7632,-7548,-7461,-7372,-7281,-7188,-7092,-6994,-6894,-6792,-6688,-6582,-6474,-6364,-6252,-6138,-6022,-5905,-5785,-5664,-5541,-5416,-5290,-5162,-5033,-4902,-4769,-4635,-4500,-4363,-4225,-4086,-3945,-3804,-3661,-3517,-3371,-3225,-3078,-2930,-2781,-2631,-2481,-2329,-2177,-2025,-1871,-1717,-1563,-1408,-1253,-1097,-941,-784,-628,-471,-314,-157};


//variables for ball sim event.
unsigned int ledball_angle;
unsigned int ledball_cval;
//ball sim controller
void ledball_crtl(void);

//RGB LED array of structures
struct rgbled {
	unsigned char r;
	unsigned char g;
	unsigned char b;
}LED[16];

struct angleinfo {
	unsigned int grad;
	unsigned int *cval;
	int *deg;
}ANGLEINFO[4];


//States of leddrive_event()
enum leddrive_states {STARTUP,BATSTATUS,ANGLE,ROTATE,GRADIENT,CLEAR,STOP,BALL,FADEANGLE,ERROR,EMERG,BUSY,COLORDIS};
//possible gradients for cval
enum leddrive_gradient {BLACKWHITE,STOPLIGHT,BLUERED,RED,GREEN,BLUE};

unsigned int leddrive_state;

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
void leddrive_rotate(int *rot);
void leddrive_clear(void);//blanks led's
void leddrive_stop(void);//keeps current color states on led's
void leddrive_gradient(unsigned int *cval1,unsigned int *cval2,unsigned int grad1,unsigned int grad2);
void leddrive_event(void);//MAIN EVEN CONTROLLER
void leddrive_ball(void);
void leddrive_fadeangle(unsigned int id,unsigned int grad,unsigned int *cval,int *deg);
void leddrive_error(void);
void leddrive_emerg(void);
void leddrive_colordisplay(void);
void leddrive_busy(void);




float tof(unsigned char);

void loop(void);
void init(void);
void display(void);
void drawCircle(double, double, double);

int main (int argc, char **argv)
{
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("Robot LED Simulator");
		init();

		glutDisplayFunc(display);
		glutIdleFunc(loop);
		glutMainLoop();
		printf("got here\n");
		return 0;
}

void loop(void) {

	struct timeval sltime;
	sltime.tv_usec = 10000;
	
	// YOUR CODE GOES HERE

	leddrive_event();

	// END YOUR CODE
	
	display();
	//select(0,NULL,NULL,NULL,&sltime);
	usleep(10000);
}

void init(void)
{	 
	leddrive_state = STARTUP;
	leddrive_startver = 3;
	int i,j;
	for(i=0,j=5000;i<=3;i++)
		(ANGLEINFO[i].cval)=&j;
	clearled();

		glClearColor(0.0, 0.0, 0.0, 0.0);
		glColor3f(1.0, 1.0, 1.0);
		glMatrixMode(GL_PROJECTION);	
		glLoadIdentity();
		glOrtho(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);
}

void display(void)
{
		glClear(GL_COLOR_BUFFER_BIT);
		
		// DRAW WHATEVER

		glColor3f(tof(LED[0].r),tof(LED[0].g),tof(LED[0].b));	 
		drawCircle(6*cos(4*C), 6*sin(4*C), 1.0);
		glColor3f(tof(LED[1].r),tof(LED[1].g),tof(LED[1].b));	
		drawCircle(6*cos(5*C), 6*sin(5*C), 1.0);
		glColor3f(tof(LED[2].r),tof(LED[2].g),tof(LED[2].b));	
		drawCircle(6*cos(6*C), 6*sin(6*C), 1.0);
		glColor3f(tof(LED[3].r),tof(LED[3].g),tof(LED[3].b));	
		drawCircle(6*cos(7*C), 6*sin(7*C), 1.0);
		glColor3f(tof(LED[4].r),tof(LED[4].g),tof(LED[4].b));	
		drawCircle(6*cos(8*C), 6*sin(8*C), 1.0);
		glColor3f(tof(LED[5].r),tof(LED[5].g),tof(LED[5].b));	
		drawCircle(6*cos(9*C), 6*sin(9*C), 1.0);
		glColor3f(tof(LED[6].r),tof(LED[6].g),tof(LED[6].b));	
		drawCircle(6*cos(10*C), 6*sin(10*C), 1.0);
		glColor3f(tof(LED[7].r),tof(LED[7].g),tof(LED[7].b));	
		drawCircle(6*cos(11*C), 6*sin(11*C), 1.0);
		glColor3f(tof(LED[8].r),tof(LED[8].g),tof(LED[8].b));	
		drawCircle(6*cos(12*C), 6*sin(12*C), 1.0);
		glColor3f(tof(LED[9].r),tof(LED[9].g),tof(LED[9].b));	
		drawCircle(6*cos(13*C), 6*sin(13*C), 1.0);
		glColor3f(tof(LED[10].r),tof(LED[10].g),tof(LED[10].b));	
		drawCircle(6*cos(14*C), 6*sin(14*C), 1.0);
		glColor3f(tof(LED[11].r),tof(LED[11].g),tof(LED[11].b));	
		drawCircle(6*cos(15*C), 6*sin(15*C), 1.0);
		glColor3f(tof(LED[12].r),tof(LED[12].g),tof(LED[12].b));	
		drawCircle(6*cos(0), 6*sin(0), 1.0);
		glColor3f(tof(LED[13].r),tof(LED[13].g),tof(LED[13].b));	
		drawCircle(6*cos(C), 6*sin(C), 1.0);
		glColor3f(tof(LED[14].r),tof(LED[14].g),tof(LED[14].b));	
		drawCircle(6*cos(2*C), 6*sin(2*C), 1.0);
		glColor3f(tof(LED[15].r),tof(LED[15].g),tof(LED[15].b));	
		drawCircle(6*cos(3*C), 6*sin(3*C), 1.0);

		// END DRAWING
		glutSwapBuffers();
}

void drawCircle(double x, double y, double radius)
{
	double angle, x1, x2, y1, y2;
	int i;
	x1 = x;
	y1 = y;
	glBegin(GL_TRIANGLES);
	for(i=0; i<=360; i += 10) {
		angle = ((double)i)/57.29577957795135;
		x2 = x + (radius*sin(angle));
		y2 = y + (radius*cos(angle));
		glVertex2d(x,y);
		glVertex2d(x1,y1);
		glVertex2d(x2,y2);
		y1=y2;
		x1=x2;
	}
	glEnd();
}


//opengl only
float tof(unsigned char a){
float b;
b= a;
b=b/255;
return b;
}


void fadein(unsigned char *light,unsigned char incr,unsigned char ending){
	if ((*light + incr) >= ending)
		*light = ending;
	else
		(*light)+=incr;
}
void fadeout(unsigned char *light,unsigned char incr,unsigned char ending){
	if ((*light - incr) <= ending)
		*light = ending;
	else
		(*light)-=incr;
}

void fadeto(struct rgbled *light, unsigned char r,unsigned char g,unsigned char b,unsigned char incr){
	if ((*light).r>r)
		fadeout(&(*light).r,incr,r);
	else
		fadein(&(*light).r,incr,r);
	if ((*light).g>g)
		fadeout(&(*light).g,incr,g);
	else
		fadein(&(*light).g,incr,g);
	if ((*light).b>b)
		fadeout(&(*light).b,incr,b);
	else
		fadein(&(*light).b,incr,b);
}

void pulse(struct rgbled *light,unsigned char r,unsigned char g,unsigned char b,unsigned char incr,unsigned int length){
	static int a;
	a++;
	if (a <= length)
		fadeto(&(*light),r,g,b,incr);
	if (a > length)
		fadeto(&(*light),0,0,0,incr);
	if (a >= 2*length)
		a=0;
}

void anglelight(int angle, unsigned char r,unsigned char g,unsigned char b){
	unsigned int light;
	while (angle>359)
		angle-=360;
	while (angle<0)
	  angle+=360;
	angle=360-angle;
	light = (23*angle+253)>>9;
	if(light>15)
		light=0;
	LED[light].r=r;
	LED[light].g=g;
	LED[light].b=b;
}


void fadeangle(int angle, unsigned char r,unsigned char g,unsigned char b){
	unsigned int light;
	while (angle>359)
		angle-=360;
	while (angle<0)
	  angle+=360;
	angle=360-angle;
	light = (23*angle+253)>>9;
	if (light>=3)
		light-=3;
	else 
		light+=13;
	int i;
	for (i=3;i>=0;--i){
		if ((((r-64*i)<0)?0:(r-64*i)+LED[light].r)>255)
			LED[light].r=255;
		else 
			LED[light].r+=((r-64*i)<0)?0:(r-64*i);
		if ((((g-64*i)<0)?0:(g-64*i)+LED[light].g)>255)
			LED[light].g=255;
		else 
			LED[light].g+=((g-64*i)<0)?0:(g-64*i);
		if ((((b-64*i)<0)?0:(b-64*i)+LED[light].b)>255)
			LED[light].b=255;
		else 
			LED[light].b+=((b-64*i)<0)?0:(b-64*i);
		light++;
		if (light>15)
			light=0;
	}
	for (i=1;i<=3;++i){
		if ((((r-64*i)<0)?0:(r-64*i)+LED[light].r)>255)
			LED[light].r=255;
		else 
			LED[light].r+=((r-64*i)<0)?0:(r-64*i);
		if ((((g-64*i)<0)?0:(g-64*i)+LED[light].g)>255)
			LED[light].g=255;
		else 
			LED[light].g+=((g-64*i)<0)?0:(g-64*i);
		if ((((b-64*i)<0)?0:(b-64*i)+LED[light].b)>255)
			LED[light].b=255;
		else 
			LED[light].b+=((b-64*i)<0)?0:(b-64*i);
		light++;
		if (light>15)
			light=0;
	}
}

void battlvl(unsigned int lvl){
	unsigned int light,i;
	light = (41*lvl>>8);
	if(light>15)
		light=15;
	for(i=15;i>light;--i){
		LED[i].b=0;
		LED[i].g=0;
		LED[i].r=0;
	}
	if (lvl >10)
	lvl-=10;
	for(i = 0;i<=light;++i){
		LED[i].b=0;
		LED[i].g=(2 * lvl);
		LED[i].r=(255-(2 * lvl));
	}
}

void gradient(unsigned char r1,unsigned char g1,unsigned char b1,unsigned char r2,unsigned char g2,unsigned char b2){
	unsigned int i,j;
	int r,g,b;

	for(i=0;i<=7;++i){
		LED[i].r=r1;
		LED[i].g=g1;
		LED[i].b=b1;
	}
	r=(r2-r1)>>3;
	g=(g2-g1)>>3;
	b=(b2-b1)>>3;

	for(i=1;i<=7;i++){
		LED[i].r+=(r*i);
		LED[i].g+=(g*i);
		LED[i].b+=(b*i);
	}
	LED[8].r=r2;
	LED[8].g=g2;
	LED[8].b=b2;
	
	for(i=1,j=14;i<=7;i++,j-=2){
		LED[i+j].r = LED[i].r;
		LED[i+j].g = LED[i].g;
		LED[i+j].b = LED[i].b;
	}
}

void rotate(int rot){
	static int a;
	unsigned char r,g,b;
	int i;
	a++;
	if (rot >0 && a>=rot){
		r =LED[0].r;
		g =LED[0].g;
		b =LED[0].b;
		for (i=0;i<=15;++i){
			LED[i].r=LED[i+1].r;
			LED[i].g=LED[i+1].g;
			LED[i].b=LED[i+1].b;
		}			
		LED[15].r=r;
		LED[15].g=g;
		LED[15].b=b; 
		a=0;
	}
	else if (rot <0 && a>=-rot){
		r =LED[15].r;
		g =LED[15].g;
		b =LED[15].b;
		for (i=15;i>=0;--i){
			LED[i].r=LED[i-1].r;
			LED[i].g=LED[i-1].g;
			LED[i].b=LED[i-1].b;
		}			
		LED[0].r=r;
		LED[0].g=g;
		LED[0].b=b;
		a=0;
	}
}
void clearled(void){
	memset(LED, 0x00, sizeof(LED));
}

void cvalselect(unsigned char *r,unsigned char *g,unsigned char *b,unsigned int grad, unsigned int cval){
	switch(grad){
	case BLACKWHITE:
		*r=cval>>4;
		*g=*b=*r;
		break;
	case STOPLIGHT:
		if (cval<2048){
		*r=cval>>3;
		*g=255;
		}
		if (cval>=2048){
		*r=255;
		*g=(255-((cval-2048)>>3));
		}
		*b=0;
		break;
	case BLUERED:
		*r=cval>>4;
		*b=255-*r;
		*g=0;
		break;
	case RED:
		*b=*g=0;
		*r=cval>>4;
		break;
	case GREEN:
		*g=cval>>4;
		*r=*b=0;
		break;
	case BLUE:
		*r=*g=0;
		*b=cval>>4;
		break;		
	}
}

void startup(void){
		static unsigned int a,i;
		a++;
	switch(leddrive_startver){
	case 1:
		a=0;
		leddrive_startver = -1;
	case -1:
		if (a<148){
			for (i=0;i<=15;++i)
				fadeto(&LED[i],255,255,255,2);
		}
		if (a>150 && a < 1100){
			for (i=0;i<=15;i+=4)
				fadeto(&LED[i],0,0,255,2);
			for (i=1;i<=15;i+=4)
				fadeto(&LED[i],0,255,0,2);
			for (i=2;i<=15;i+=4)
				fadeto(&LED[i],255,0,0,2);
			for (i=3;i<=15;i+=4)
				fadeto(&LED[i],255,255,0,2);
		}
		if(a>300 && a<900)
			rotate(48);
	
		if(a>1100 && a<1250){
			for (i=0;i<=15;++i)
				fadeto(&LED[i],0,0,0,2);
		}
		if (a>1250)		
			leddrive_batstatus();
		break;
	case 2:
		a=0;
		leddrive_startver = -2;
	case -2:
		if (a<10)
			gradient(14,90,33,0,0,100);
		if (a>=50 && a<=740)
			rotate(5);
		if(a>=250&& a<600)
 			fadeto(&LED[0],255,0,0,5);
		if (a>=400&& a<600)
			fadeto(&LED[0],255,0,0,17);
		if (a==600||a==620||a==640||a==660)
			LED[0].g=128;
		if (a>780 && a<920){
			for (i=0;i<=15;++i)
				fadeto(&LED[i],0,0,0,2);
		}
		if(a>920)
			leddrive_batstatus();
		break;
	case 3:
	default:
		a=0;
		i=0;
		leddrive_startver=-3;
	case -3:
		if (a==1){
			LED[0].r=255;
			LED[0].g=204;
		}
		if (a<=80)
			rotate(5);
		if (a>110 && a<= 190)
			rotate(-5);
		if (a>210 && a<= 290)
			rotate(5);
		if (a==210){
			LED[8].g=102;
			LED[8].b=51;
		}
		if (a==230){
			LED[0].r=LED[8].r=124;
			LED[0].g=LED[8].g=150;
			LED[0].b=LED[8].b=23;
		}
		if (a>310 && a<= 390)
			rotate(-5);
		if (a==460)
			gradient(255,204,0,0,102,51);
		if (a>=500&& a<800){
			i++;
			switch (i){
			case 1:
				gradient(255,204,0,0,102,51);
				break;
			case 31:
				gradient(0,102,51,255,204,0);	
				break;
			case 60:
				i=0;
				break;
			}
		}
		if (a>=800&&a<960){
			for (i=0;i<=15;i+=2)
				fadeto(&LED[i],255,204,0,3);
			for (i=1;i<=15;i+=2)
				fadeto(&LED[i],0,102,51,3);	
			}
		if(a>=960&&a<1280){
			rotate(20);
			fadeto(&LED[0],0,0,0,3);
		}
		if(a>=1280&& a<1600){
			rotate(-20);
			fadeto(&LED[0],0,0,0,10);
		}
		if(a>= 1650)
			leddrive_batstatus();
		break;
	}	
}
// MAIN EVENT CONTROLLER
void leddrive_event(void) {
  unsigned static int old_leddrive_gradcval1,old_leddrive_gradcval2;
	unsigned char r,g,b,r1,g1,b1;
	int i,j;
	static int a;
	
	switch (leddrive_state){
	case STARTUP:
		startup();
		break;
	case BATSTATUS:
		clearled();
		battlvl(leddrive_batlvl);//
		break;
	case BALL:
  	ledball_crtl();	
  	ANGLEINFO[0].deg=&ledball_angle;
    ANGLEINFO[0].cval=&ledball_cval;
    ANGLEINFO[0].grad=BLUERED;
    *(ANGLEINFO[1].cval)=*(ANGLEINFO[2].cval)=*(ANGLEINFO[3].cval)=5000;
	case ANGLE:
		clearled();
		for(i=0;i<=3;i++){
			if (*(ANGLEINFO[i].cval)<4096){
				cvalselect(&r,&g,&b,ANGLEINFO[i].grad,*(ANGLEINFO[i].cval));
				anglelight(*(ANGLEINFO[i].deg),r,g,b);
			}
		}	
		break;
	case ROTATE:
		rotate(*leddrive_rot);
		break;
	case GRADIENT:
		if(old_leddrive_gradcval1 == *leddrive_gradcval1 && old_leddrive_gradcval2 == *leddrive_gradcval2)
			break;
		clearled();
		cvalselect(&r,&g,&b,leddrive_grad1,*leddrive_gradcval1);
		cvalselect(&r1,&g1,&b1,leddrive_grad2,*leddrive_gradcval2);
		gradient(r,g,b,r1,g1,b1);
		old_leddrive_gradcval1 = *leddrive_gradcval1;
		old_leddrive_gradcval2 = *leddrive_gradcval2;
		break;
	case FADEANGLE:
		clearled();
		for(i=0;i<=3;i++){
			if (*(ANGLEINFO[i].cval)<4096){
				cvalselect(&r,&g,&b,ANGLEINFO[i].grad,*(ANGLEINFO[i].cval));
				fadeangle(*(ANGLEINFO[i].deg),r,g,b);
			}
		}
		break;
	case ERROR:
		a++;
		if (a==1){
			for(i=0;i<=15;i++){
				LED[i].r=255;					
				LED[i].g=128;
			}
		}
		if (a==15){
			for(i=0;i<=15;i++)
				LED[i].g=0;
		}
		if (a==30)
			a=0;
		break;
	case EMERG:
		a++;
		if (a==1){
		for(i=0;i<=15;i+=2)
			LED[i].r=255;
		for(j=1;j<=15;j+=2)
			LED[j].b=255;
		}
		rotate(25);
		break;
	case BUSY:
		a++;
		LED[(a>>3)].g=80+(a);
		if ((a>>3)>15){
		a=0;
		clearled();
		}
		break;
	case COLORDIS:
			rotate(15);
			a++;
			if (a==1){
				clearled();
				gradient(255,0,0,0,0,0);
			}	
			if (a>50 && a<600)
				fadein(&LED[0].g,5,255);
			if (a>600 && a<1200)
				fadein(&LED[0].b,5,255);
			if (a>900 && a<1200)
				fadeout(&LED[0].r,5,0);
			if (a>1200)
				fadeout(&LED[0].g,5,0);
			if (a>1500)
				fadein(&LED[0].r,5,255);
			if (a>2000)
				fadeout(&LED[0].b,7,0);
			if (a==2400)
				a=0;
		break;
  case CLEAR:	
    clearled();
    leddrive_stop();
    break;
  case STOP:
		break;
	}
}

//external
void leddrive_batstatus(void){
	leddrive_state = BATSTATUS;
}

void leddrive_angle(unsigned int id,unsigned int grad,unsigned int *cval,int *deg){
	leddrive_state= ANGLE;
	ANGLEINFO[id].grad=grad;
	ANGLEINFO[id].deg=deg;
	ANGLEINFO[id].cval=cval;
}

void leddrive_rotate(int *rot){
	leddrive_state=ROTATE;
	leddrive_rot = rot;
}

void leddrive_clear(void){
	leddrive_state=CLEAR;
}

void leddrive_stop(void){
	leddrive_state=STOP;
}

void leddrive_gradient(unsigned int *cval1,unsigned int *cval2,unsigned int grad1,unsigned int grad2){
	clearled();
	leddrive_state=GRADIENT;
	leddrive_gradcval1=cval1;
	leddrive_gradcval2=cval2;
	leddrive_grad1=grad1;
	leddrive_grad2=grad2;
}

void leddrive_startup(int ver){
	clearled();
	leddrive_state=STARTUP;
	leddrive_startver = ver;
}

void leddrive_ball(void){
	clearled();
	leddrive_state=BALL;
}

void leddrive_error(void){
	clearled();
	leddrive_state=ERROR;
}
void leddrive_emerg(void){
	clearled();
	leddrive_state=EMERG;
}
void leddrive_colordisplay(void){
	clearled();
	leddrive_state=COLORDIS;
}
void leddrive_busy(void){
	clearled();
	leddrive_state=BUSY;
}

void leddrive_fadeangle(unsigned int id,unsigned int grad,unsigned int *cval,int *deg){
	leddrive_state=FADEANGLE;
	ANGLEINFO[id].grad=grad;
	ANGLEINFO[id].deg=deg;
	ANGLEINFO[id].cval=cval;
}



/* 
*
*LED Ball Simulator
*
*/



#define T 0.01// time constant
#define PI 3.141592654

int meu= 20;

int ledball_gx;
int ledball_gy;
int velobot;

int angle;
int veloball=180000;
int accelball;
int fricaccel;
int accely,accelx,accelrot;

void ledball_crtl(void){

	if (veloball >0)
		fricaccel=-meu*9000;
	else if (veloball<0)
		fricaccel=meu*9000;
	else
		fricaccel=0;

	accely=-(ledball_gy)*(cos_lookup[ledball_angle]);
	accelx=-(ledball_gx)*(sin_lookup[ledball_angle]);


	accelrot=((meu*velobot)>>3);
	if ((abs(velobot)-abs(veloball))<2)
		accelrot=0;

	accelball=(fricaccel + accely + accelx +accelrot);
	veloball= veloball +(accelball*T);
	angle=angle+((veloball-velobot)*T);
	
	ledball_cval=4095;//color of ball, can change to be variable
		
	if (angle>359000)
		angle-=360000;
	if (angle<0)
	  angle+=360000;	
	ledball_angle = angle/1000;	     
}


