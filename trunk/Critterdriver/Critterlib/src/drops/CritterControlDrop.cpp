#include <stdio.h>
#include <string.h>
#include <iostream>
#include "CritterControlDrop.h"


std::string CritterControlDrop::name = "CritterControlDrop";

CritterControlDrop::CritterControlDrop() {
	reset();
}

void CritterControlDrop::reset() {
	  avr_commands = 0;
	  motor_mode = WHEEL_VOLTAGE;
	  m100 = 0;
	  m220 = 0;
	  m340 = 0;
	  led_mode = BUSY;
}

int CritterControlDrop::getDropSize() {
	return 4 + name.length() + 4 + getDataSize();
}

int CritterControlDrop::getDataSize() {
  return sizeof(motor_mode) +
         sizeof(m100) +
         sizeof(m220) +
         sizeof(m340) +
         sizeof(led_mode) +
         sizeof(led_val);
}

void CritterControlDrop::readArray(void *d) {
  int i = 0;
  char *data = (char *)d + 4 + name.length() + 4;
  memcpy(&motor_mode, data + i, sizeof(motor_mode)); i += sizeof(motor_mode);
  memcpy(&m100, data + i, sizeof(m100));   i += sizeof(m100);
  memcpy(&m220, data + i, sizeof(m220));   i += sizeof(m220);
  memcpy(&m340, data + i, sizeof(m340));   i += sizeof(m340);
  memcpy(&led_mode, data + i, sizeof(led_mode));   i += sizeof(led_mode);
  memcpy(&led_val,  data + i, sizeof(led_val));  i += sizeof(led_val);
}


CritterControlDrop::~CritterControlDrop() {
}
