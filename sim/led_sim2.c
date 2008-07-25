



struct rgbled {
	unsigned short r;
	unsigned short g;
	unsigned short b;
}LED[16];

enum led_states {STARTUP,BATSTATUS,ANGLE,ROTATE,GRADIENT,CLEAR,STOP};

int led_state;
int led_startver;
int *led_angledeg;
int *led_anglecval;
int *led_gradcval1;
int *led_gradcval2;
int led_batlvl;

void fadein(unsigned short *,unsigned short,unsigned short);
void fadeout(unsigned short *,unsigned short,unsigned short);
void fadeto(struct rgbled *,unsigned short,unsigned short,unsigned short,unsigned short);
void anglelight(int, unsigned short, unsigned short, unsigned short);
void battlvl(int);
void gradient(unsigned short,unsigned short,unsigned short,unsigned short,unsigned short,unsigned short);
void rotate(void);
void startup(void);
void clearled(void);


void led_ctrl(void);


void led_startup(int);
void led_batstatus(void);
void led_angle(int *,int *);
void led_rotate(void);
void led_clear(void);
void led_stop(void);
void led_gradient(int *,int *);



void loop(void);
void init(void);

void loop(void) {

	struct timeval sltime;
	sltime.tv_usec = 10000;
	
	// YOUR CODE GOES HERE

	
		
	led_ctrl();	



	// END YOUR CODE
	
	//select(0,NULL,NULL,NULL,&sltime);
	usleep(10000);
}

void init(void)
{	 
		led_state = STARTUP;
		led_startver = 2;
}



void fadein(unsigned short *light,unsigned short incr,unsigned short ending){
	if ((*light+=incr) > ending)
		*light = ending;
}
void fadeout(unsigned short *light,unsigned short incr,unsigned short ending){
	if ((*light - incr) <= ending)
		*light = ending;
	else
		(*light)-=incr;
}

void fadeto(struct rgbled *light, unsigned short r,unsigned short g,unsigned short b,unsigned short incr){
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

void anglelight(int angle, unsigned short r,unsigned short g,unsigned short b){
	int light;
	while (angle>359)
		angle-=360;
	angle=360-angle;
	light = (angle+11.25)/22.5;
	if(light>15)
		light=0;
	LED[light].r=r;
	LED[light].g=g;
	LED[light].b=b;
}
void battlvl(int lvl){
	int light,i;
	light = ((lvl+3)/6.25);
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
		LED[i].g=(10.23 * lvl);
		LED[i].r=(1023-(10.23 * lvl));
	}
}

void gradient(unsigned short r1,unsigned short g1,unsigned short b1,unsigned short r2,unsigned short g2,unsigned short b2){
	int i,j;
	int r,g,b;

	for(i=0;i<=15;++i){
		LED[i].r=r1;
		LED[i].g=g1;
		LED[i].b=b1;
	}
	r=(r2-r1)/8;
	g=(g2-g1)/8;
	b=(b2-b1)/8;
	for(i=1;i<=8;i++){
		LED[i].r+=(r*i);
		LED[i].g+=(g*i);
		LED[i].b+=(b*i);
	}
	for(i=1,j=14;i<=15;i++,j-=2){
		LED[i+j].r = LED[i].r;
		LED[i+j].g = LED[i].g;
		LED[i+j].b = LED[i].b;
	}
}

void rotate(void){
	int i,r,g,b;
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
}

void clearled(void){
	int i;
	for (i=0;i<=15;++i){
	LED[i].r=0;
	LED[i].g=0;
	LED[i].b=0;
	}
}

void startup(void){
		static int a,c,i;
		a++;
		c++;
	switch(led_startver){

	case 1:
	default:
		a=0;
		led_startver = -1;
	case -1:
		if (a<148){
			for (i=0;i<=15;++i)
				fadeto(&LED[i],1023,1023,1023,7);
		}
		if (a>150 && a < 1100){
			for (i=0;i<=15;i+=4)
				fadeto(&LED[i],0,0,1023,7);
			for (i=1;i<=15;i+=4)
				fadeto(&LED[i],0,1023,0,7);
			for (i=2;i<=15;i+=4)
				fadeto(&LED[i],1023,0,0,7);
			for (i=3;i<=15;i+=4)
				fadeto(&LED[i],1023,1023,0,7);
		}
		if(a>300 && c>48 && a<900){
			rotate();
			c=0;
		}

		if(a>1100 && a<1250){
			for (i=0;i<=15;++i)
				fadeto(&LED[i],0,0,0,7);
		}
		if (a>1250)		
			led_batstatus();
		break;
	case 2:
		a=0;
		led_startver = -2;
	case -2:
	
		if (a<10)
			gradient(55,361,130,0,0,400);
		if (a>=50 && c>=5 && a<=740){
			rotate();
			c=0;
		}
		if(a>=250&& a<600)
 			fadeto(&LED[0],1023,0,0,20);
		if (a>=400&& a<600)
			fadeto(&LED[0],1023,0,0,70);
		if (a==600||a==620||a==640||a==660)
			LED[0].g=662;
		if (a>780 && a<920){
			for (i=0;i<=15;++i)
				fadeto(&LED[i],0,0,0,7);
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
	static int a;
	int r,b,r2,b2;
	
	switch (led_state){
	case STARTUP:
		startup();
		break;
	case BATSTATUS:
		clearled();
		battlvl(led_batlvl);//global var.
		break;
	case ANGLE:
    clearled();
		r=(*led_anglecval)/4;
		b=1023-r;
		anglelight(*led_angledeg,r,0,b);
		break;
	case ROTATE:
		a++;
		if (a>=15){
			rotate();
			a=0;
		}
		break;
	case GRADIENT:
		clearled();
		r=(*led_gradcval1)/4;
		b=1023-r;
		r2=(*led_gradcval2)/4;
		b2=1023-r2;
		gradient(r,0,b,r2,0,b2);
		break;
  case CLEAR:
    clearled();
    break;
  case STOP:
  	break;
	}
}

//external
void led_batstatus(void){
	led_state = BATSTATUS;
}

void led_angle(int *deg,int *cval){
	led_state= ANGLE;
	led_angledeg = deg;
	led_anglecval = cval;
}

void led_rotate(void){
	led_state=ROTATE;
}

void led_clear(void){
	led_state=CLEAR;
}

void led_stop(void){
	led_state=STOP;
}

void led_gradient(int *cval1, int *cval2){
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
