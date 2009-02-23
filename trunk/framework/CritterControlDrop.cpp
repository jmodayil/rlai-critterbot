#include "CritterControlDrop.h"


string CritterControlDrop::name = "CritterControlDrop";

CritterControlDrop::CritterControlDrop() {

}

CritterControlDrop::~CritterControlDrop() {

}

int CritterControlDrop::getSize() {

  return 20 + sizeof(led_val);

}

void CritterControlDrop::writeArray(void *d) {

  int i = 0;
  char *data = (char *)d;
  memcpy(data + i, &motor_mode, 4); i += 4;
  memcpy(data + i, &m100_vel, 4);   i += 4;
  memcpy(data + i, &m220_vel, 4);   i += 4;
  memcpy(data + i, &m340_vel, 4);   i += 4;
  memcpy(data + i, &led_mode, 4);   i += 4;
  memcpy(data + i, &led_val, sizeof(led_val));  i += sizeof(led_val);
}

void CritterControlDrop::readArray(void *d) {
  
  int i = 0;
  char *data = (char *)d;
  memcpy(&motor_mode, data + i, 4); i += 4;
  memcpy(&m100_vel, data + i, 4);   i += 4;
  memcpy(&m220_vel, data + i, 4);   i += 4;
  memcpy(&m340_vel, data + i, 4);   i += 4;
  memcpy(&led_mode, data + i, 4);   i += 4;
  memcpy(&led_val,  data + i, sizeof(led_val));  i += sizeof(led_val);
}

string CritterControlDrop::toString(string offset) {

  string out;
  char buf[100];

  int i;
  out += "- Critter Control Drop -\n";
  sprintf(buf, "motor_mode: %d\n", motor_mode);
  out += buf;
  sprintf(buf, "m100_vel: \t %d m220_vel: \t %d m340_vel: \t %d\n", m100_vel, m220_vel, m340_vel);
  out += buf;
  sprintf(buf,"led_mode: %d\n", led_mode);
  out += buf;
  if(led_mode == CUSTOM) {
    for( i = 0; i < NUM_LEDS; i++ ) {
      out += "*";
    }
  }
  printf("Size of led_val: %d\n", sizeof(led_val)); 
  out += "\n";
  return out; 
}

string CritterControlDrop::getName() {
  return name;
}
