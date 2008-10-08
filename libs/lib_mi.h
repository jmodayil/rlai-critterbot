#ifndef LIB_MI_H
#define LIB_MI_H

#define MI_HEADER1 0xDE
#define MI_HEADER2 0xAD
#define MI_HEADER3 0xBE
#define MI_HEADER4 0xEF

#define MI_COMMAND_LENGTH 9

#define XSC100 (-1008) // cos(90+100) * 1024
#define YSC100 (-178) // sin(90+100) * 1024
#define TSC100 (1096) // 1.07 * 1024
#define XSC220 (658) // cos(90+220) * 1024
#define YSC220 (-784) // sin(90+220) * 1024
#define TSC220 (1096) // 1.07 * 1024
#define XSC340 (350) // cos(90+340) * 1024
#define YSC340 (962) // sin(90+340) * 1024
#define TSC340 (1096) // 1.07 * 1024

#define ABS(a) (a >= 0 ? a : -a)

void mi_send_status(void);
void mi_get_commands(void);
void mi_start(void);
void mi_stop(void);
int mi_event(void);
void putwcrc(unsigned char data);

struct command_packet {

  enum {WHEEL_SPACE, XYTHETA_SPACE } motor_mode;
  union {
    int m100_vel;
    int x_vel;
  };
  union {
    int m220_vel;
    int y_vel;
  };
  union {
    int m340_vel;
    int theta_vel;
  };

  enum { THING1, THING2, THING3 } led_mode;
};


#endif
