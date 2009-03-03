#include "CritterControlDrop.h"


string CritterControlDrop::name = "CritterControlDrop";

CritterControlDrop::CritterControlDrop() {

}

CritterControlDrop::~CritterControlDrop() {

}

int CritterControlDrop::getSize() {

  return sizeof(motor_mode) + 
         sizeof(m100_vel) + 
         sizeof(m220_vel) + 
         sizeof(m340_vel) +
         sizeof(led_mode) +
         sizeof(led_val);

}

void CritterControlDrop::writeArray(void *d) {

  int i = 0;
  char *data = (char *)d;
  memcpy(data + i, &motor_mode, sizeof(motor_mode)); i += sizeof(motor_mode);
  memcpy(data + i, &m100_vel, sizeof(m100_vel));   i += sizeof(m100_vel);
  memcpy(data + i, &m220_vel, sizeof(m220_vel));   i += sizeof(m220_vel);
  memcpy(data + i, &m340_vel, sizeof(m340_vel));   i += sizeof(m340_vel);
  memcpy(data + i, &led_mode, sizeof(led_mode));   i += sizeof(led_mode);
  memcpy(data + i, &led_val, sizeof(led_val));  i += sizeof(led_val);
}

void CritterControlDrop::readArray(void *d) {
  
  int i = 0;
  char *data = (char *)d;
  memcpy(&motor_mode, data + i, sizeof(motor_mode)); i += sizeof(motor_mode);
  memcpy(&m100_vel, data + i, sizeof(m100_vel));   i += sizeof(m100_vel);
  memcpy(&m220_vel, data + i, sizeof(m220_vel));   i += sizeof(m220_vel);
  memcpy(&m340_vel, data + i, sizeof(m340_vel));   i += sizeof(m340_vel);
  memcpy(&led_mode, data + i, sizeof(led_val));   i += sizeof(led_val);
  memcpy(&led_val,  data + i, sizeof(led_val));  i += sizeof(led_val);
}

string CritterControlDrop::toString(string offset) {

  string out;
  char buf[1024];

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
  out += "\n";
  return out; 
}

string CritterControlDrop::getName() {
  return name;
}
