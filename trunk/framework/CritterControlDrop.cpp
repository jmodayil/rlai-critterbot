#include "CritterControlDrop.h"


string CritterControlDrop::name = "CritterControlDrop";

CritterControlDrop::CritterControlDrop() {

}

CritterControlDrop::~CritterControlDrop() {

}

int CritterControlDrop::getSize() {

  return 20; 

}

void CritterControlDrop::writeArray(void *d) {

  int i = 0;
  char *data = (char *)d;
  memcpy(data + i, &motor_mode, 4); i += 4;
  memcpy(data + i, &m100_vel, 4);   i += 4;
  memcpy(data + i, &m220_vel, 4);   i += 4;
  memcpy(data + i, &m340_vel, 4);   i += 4;
  memcpy(data + i, &led_mode, 4);   i += 4;
  
}

void CritterControlDrop::readArray(void *d) {
  
  int i = 0;
  char *data = (char *)d;
  memcpy(&motor_mode, data + i, 4); i += 4;
  memcpy(&m100_vel, data + i, 4);   i += 4;
  memcpy(&m220_vel, data + i, 4);   i += 4;
  memcpy(&m340_vel, data + i, 4);   i += 4;
  memcpy(&led_mode, data + i, 4);   i += 4;
  
}

string CritterControlDrop::toString(string offset) {

  printf("- Critter Control Drop -\n");
  
  printf("motor_mode: %d\n", motor_mode);
  printf("m100_vel: \t %d m220_vel: \t %d m340_vel: \t %d \n\n", m100_vel, m220_vel, m340_vel);
  
}

string CritterControlDrop::getName() {
  return name;
}
