#include "CritterStateDrop.h"

string CritterStateDrop::name = "CritterStateDrop";

CritterStateDrop::CritterStateDrop() {
  bus_voltage = 0;
  batv40 = 0;
  batv160 = 0;
  batv280 = 0;
  accel.x = 0;
  accel.y = 0;
  accel.z = 0;
  mag.x = 0;
  mag.y = 0;
  mag.z = 0;
  motor100.velocity = 0;
  motor100.current = 0;
  motor100.temp = 0;
  motor220.velocity = 0;
  motor220.current = 0;
  motor220.temp = 0;
  motor340.velocity = 0;
  motor340.current = 0;
  motor340.temp = 0;
  
}

CritterStateDrop::~CritterStateDrop() {
  
}

int CritterStateDrop::getSize() {
  return 324; 
}

void CritterStateDrop::writeArray(void *d) {
  
  int i = 0;
  char *data = (char *)d;
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

void CritterStateDrop::readArray(void *d) {

  int i = 0;
  char *data = (char *)d;
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


  printf("- Critter State Drop -\n");

  printf("Power_Source: %d \t", power_source);
  printf("bus_voltage: %d \t", (int)bus_voltage);
  printf("batv [40,160,280]: [%d,%d,%d] \n", (int)batv40, (int)batv160, (int)batv280);

  printf("motor | \tvelocity, \tcurrent, \ttemp\n");
  printf("motor100 | \t %d \t %d \t %d \n", motor100.velocity, motor100.current, motor100.temp);
  printf("motor220 | \t %d \t %d \t %d \n", motor220.velocity, motor220.current, motor220.temp);
  printf("motor340 | \t %d \t %d \t %d \n", motor340.velocity, motor340.current, motor340.temp);

  printf("accel (x,y,z): (%d,%d,%d) \t mag (x,y,z): (%d,%d,%d)\n",accel.x,accel.y,accel.z,mag.x,mag.y,mag.z);
  printf("rotation: %d \n ir_dists: ",rotation);
  for(int i=0; i<10; i++) printf(" [%d]: %d ", i, ir_distance[i]);
  printf("\n ir_light: ");
  for(int i=0; i<8; i++) printf(" [%d]: %d ", i, ir_light[i]);
  printf("\n light: ");
  for(int i=0; i<4; i++) printf(" [%d]: %d ", i, light[i]);
  printf("\n thermal: ");
  for(int i=0; i<8; i++) printf(" [%d]: %d ", i, thermal[i]);
  printf("\n bump: ");
  for(int i=0; i<32; i++) printf(" [%d]: %d ", i, bump[i]);
  printf("\n error_flags: %d\n\n", error_flags);

}
