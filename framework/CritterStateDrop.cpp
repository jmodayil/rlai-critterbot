#include "CritterStateDrop.h"

string CritterStateDrop::name = "CritterStateDrop";

CritterStateDrop::CritterStateDrop() {
  
}

CritterStateDrop::~CritterStateDrop() {
  
}

int CritterStateDrop::getSize() {
  return 324; 
}

void CritterStateDrop::writeArray(void *data) {
  
  int i = 0;
  memcpy(data + i, &power_source, 4); i += 4;
  memcpy(data + i, &bus_voltage, 1);  i += 1;
  memcpy(data + i, &batv40, 1);       i += 1;
  memcpy(data + i, &batv160, 1);      i += 1;
  memcpy(data + i, &batv280, 1);      i += 1;
  
  memcpy(data + i, &motor100, 12);    i += 12;
  memcpy(data + i, &motor220, 12);    i += 12;
  memcpy(data + i, &motor340, 12);    i += 12;
  
  memcpy(data + i, &accel, 12);       i += 12;
  memcpy(data + i, &mag, 12);         i += 12;

  memcpy(data + i, &rotation, 4);     i += 4;
  memcpy(data + i, &ir_distance, 40); i += 40;
  memcpy(data + i, &ir_light, 32);    i += 32;
  memcpy(data + i, &light, 16);       i += 16;
  memcpy(data + i, &thermal, 32);     i += 32;
  memcpy(data + i, &bump, 128);       i += 128;
  memcpy(data + i, &error_flags, 4);  i += 4;

}

void CritterStateDrop::readArray(void *data) {

  int i = 0;
  memcpy(&power_source, data + i, 4); i += 4;
  memcpy(&bus_voltage, data + i, 1);  i += 1;
  memcpy(&batv40, data + i, 1);       i += 1;
  memcpy(&batv160, data + i, 1);      i += 1;
  memcpy(&batv280, data + i, 1);      i += 1;
  
  memcpy(&motor100, data + i, 12);    i += 12;
  memcpy(&motor220, data + i, 12);    i += 12;
  memcpy(&motor340, data + i, 12);    i += 12;
  
  memcpy(&accel, data + i, 12);       i += 12;
  memcpy(&mag, data + i, 12);         i += 12;

  memcpy(&rotation, data + i, 4);     i += 4;
  memcpy(&ir_distance, data + i, 40); i += 40;
  memcpy(&ir_light, data + i, 32);    i += 32;
  memcpy(&light, data + i, 16);       i += 16;
  memcpy(&thermal, data + i, 32);     i += 32;
  memcpy(&bump, data + i, 128);       i += 128;
  memcpy(&error_flags, data + i, 4);  i += 4;

}

string CritterStateDrop::toString(string offset) {


  printf("TODO: Write CritterStateDrop::toString(string offset) :P\n");

}

string CritterStateDrop::getName() {
  return name;
}
