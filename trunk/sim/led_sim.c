#include <GL/gl.h>
#include <GL/glut.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define C 0.3925

struct rgbled {
	unsigned char r;
	unsigned char g;
	unsigned char b;
}LED[16];

enum led_states {STARTUP,BATSTATUS,ANGLE,ROTATE,GRADIENT,CLEAR,STOP};

unsigned int led_state;
int *led_rot;
int led_startver;
unsigned int led_batlvl;
unsigned int *led_angledeg;
unsigned int *led_anglecval;
unsigned int *led_gradcval1;
unsigned int *led_gradcval2;


void fadein(unsigned char *,unsigned char,unsigned char);
void fadeout(unsigned char *,unsigned char,unsigned char);
void fadeto(struct rgbled *,unsigned char,unsigned char,unsigned char,unsigned char);
void pulse(struct rgbled *,unsigned char,unsigned char,unsigned char,unsigned char,unsigned int);
void anglelight(unsigned int, unsigned char, unsigned char, unsigned char);
void battlvl(unsigned int);
void gradient(unsigned char,unsigned char,unsigned char,unsigned char,unsigned char,unsigned char);
void rotate(int);
void startup(void);
void clearled(void);


void led_ctrl(void);


void led_startup(int);
void led_batstatus(void);
void led_angle(unsigned int *,unsigned int *);
void led_rotate(int *);
void led_clear(void);
void led_stop(void);
void led_gradient(unsigned int *,unsigned int *);

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
	//led_ctrl();
	static int a;
	a++;
	pulse(&LED[0],255,128,0,5,100);

	
	// END YOUR CODE
	
	display();
	//select(0,NULL,NULL,NULL,&sltime);
	usleep(10000);
}

void init(void)
{	 
		led_state = STARTUP;
		led_startver = 1;

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

//custom functions
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

void anglelight(unsigned int angle, unsigned char r,unsigned char g,unsigned char b){
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

void startup(void){
		static unsigned int a,i;
		a++;
	switch(led_startver){
	case 1:
	default:
		a=0;
		led_startver = -1;
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
			led_batstatus();
		break;
	case 2:
		a=0;
		led_startver = -2;
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
			led_batstatus();
		break;
	case 3:
		a=0;
		led_startver=-3;
	case -3:
		//3rd here
		break;
	}	
}

void led_ctrl(void) {
	unsigned static char old_led_anglecval, old_led_angledeg;
  unsigned static char old_led_gradcval1,old_led_gradcval2;
	unsigned int r,b,r2,b2;
	
	switch (led_state){
	case STARTUP:
		startup();
		break;
	case BATSTATUS:
		clearled();
		battlvl(led_batlvl);//global var.
		break;
	case ANGLE:
		if(old_led_anglecval == *led_anglecval && old_led_angledeg == *led_angledeg)
			break;
    clearled();
		r=(*led_anglecval)>>4;
		b=255-r;
		anglelight(*led_angledeg,r,0,b);
		old_led_anglecval = *led_anglecval;
		old_led_angledeg = *led_angledeg;
		break;
	case ROTATE:
		rotate(*led_rot);
		break;
	case GRADIENT:
		if(old_led_gradcval1 == *led_gradcval1 && old_led_gradcval2 == *led_gradcval2)
			break;
		clearled();
		r=(*led_gradcval1)>>4;
		b=255-r;
		r2=(*led_gradcval2)>>4;
		b2=255-r2;
		gradient(r,0,b,r2,0,b2);
		old_led_gradcval1 = *led_gradcval1;
		old_led_gradcval2 = *led_gradcval2;
		break;
  case CLEAR:	
    clearled();
    led_stop();
    break;
  case STOP:
  	break;
	}
}

//external
void led_batstatus(void){
	led_state = BATSTATUS;
}

void led_angle(unsigned int *deg,unsigned int *cval){
	led_state= ANGLE;
	led_angledeg = deg;
	led_anglecval = cval;
}

void led_rotate(int *rot){
	led_state=ROTATE;
	led_rot = rot;
}

void led_clear(void){
	led_state=CLEAR;
}

void led_stop(void){
	led_state=STOP;
}

void led_gradient(unsigned int *cval1,unsigned int *cval2){
	clearled();
	led_state=GRADIENT;
	led_gradcval1=cval1;
	led_gradcval2=cval2;
}

void led_startup(int ver){
	clearled();
	led_state=STARTUP;
	led_startver = ver;
}
