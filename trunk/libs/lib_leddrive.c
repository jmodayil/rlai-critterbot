
#include <string.h>
#include "lib_leddrive.h"

void leddrive_init(void)
{	 
	leddrive_state = STARTUP;
	leddrive_startver = 2;
	clearled();
}

void leddrive_write(void){
	int i;
	for (i=0;i<=15;i++)
		ledctl_setcolor(i,LED[i].r<<4,LED[i].g<<4,LED[i].b<<4);
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
	switch(leddrive_startver){
	case 1:
	default:
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
		a=0;
		leddrive_startver=-3;
	case -3:
		//3rd startup sequence here
		break;
	}	
}
// MAIN EVENT CONTROLLER
void leddrive_event(void) {
	unsigned static char old_leddrive_anglecval, old_leddrive_angledeg;
  unsigned static char old_leddrive_gradcval1,old_leddrive_gradcval2;
	unsigned int r,b,r2,b2;
	
	switch (leddrive_state){
	case STARTUP:
		startup();
		break;
	case BATSTATUS:
		clearled();
		battlvl(leddrive_batlvl);//
		break;
	case ANGLE:
		if(old_leddrive_anglecval == *leddrive_anglecval && old_leddrive_angledeg == *leddrive_angledeg)
			break;
    clearled();
		r=(*leddrive_anglecval)>>4;
		b=255-r;
		anglelight(*leddrive_angledeg,r,0,b);
		old_leddrive_anglecval = *leddrive_anglecval;
		old_leddrive_angledeg = *leddrive_angledeg;
		break;
	case ROTATE:
		rotate(*leddrive_rot);
		break;
	case GRADIENT:
		if(old_leddrive_gradcval1 == *leddrive_gradcval1 && old_leddrive_gradcval2 == *leddrive_gradcval2)
			break;
		clearled();
		r=(*leddrive_gradcval1)>>4;
		b=255-r;
		r2=(*leddrive_gradcval2)>>4;
		b2=255-r2;
		gradient(r,0,b,r2,0,b2);
		old_leddrive_gradcval1 = *leddrive_gradcval1;
		old_leddrive_gradcval2 = *leddrive_gradcval2;
		break;
  case CLEAR:	
    clearled();
    leddrive_stop();
    break;
  case STOP:
  	break;
	}
	leddrive_write();//sends LED[] values to the leds
}

//external
void leddrive_batstatus(void){
	leddrive_state = BATSTATUS;
}

void leddrive_angle(unsigned int *deg,unsigned int *cval){
	leddrive_state= ANGLE;
	leddrive_angledeg = deg;
	leddrive_anglecval = cval;
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

void leddrive_gradient(unsigned int *cval1,unsigned int *cval2){
	clearled();
	leddrive_state=GRADIENT;
	leddrive_gradcval1=cval1;
	leddrive_gradcval2=cval2;
}

void leddrive_startup(int ver){
	clearled();
	leddrive_state=STARTUP;
	leddrive_startver = ver;
}
