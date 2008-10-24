#include "CritterStateDrop.h"

string CritterStateDrop::name = "CritterStateDrop";

CritterStateDrop::CritterStateDrop() {
  power_source = SHORE;
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
  cycle_time = 0;

}

CritterStateDrop::~CritterStateDrop() {
  
}

int CritterStateDrop::getSize() {
  return sizeof(power_source) +
         sizeof(bus_voltage) +
         sizeof(batv40) + sizeof(batv160) + sizeof(batv280) +
         sizeof(motor100) + sizeof(motor220) + sizeof(motor340) +
         sizeof(accel) + sizeof(mag) + 
         sizeof(rotation) +
         sizeof(ir_distance) +
         sizeof(ir_light) + 
         sizeof(light) + 
         sizeof(thermal) +
         sizeof(bump) +
         sizeof(error_flags) +
         sizeof(cycle_time);
}

void CritterStateDrop::writeArray(void *d) {
  
  char *data = (char *)d;
  memcpy(data, &power_source, sizeof(power_source));
  data += sizeof(power_source);
  
  memcpy(data, &bus_voltage, sizeof(bus_voltage));
  data += sizeof(bus_voltage);
  
  memcpy(data, &batv40, sizeof(batv40));
  data += sizeof(batv40);
  
  memcpy(data, &batv160, sizeof(batv160));
  data += sizeof(batv160);
  
  memcpy(data, &batv280, sizeof(batv280));
  data += sizeof(batv280);
  
  memcpy(data, &motor100, sizeof(motor100));
  data += sizeof(motor100);
  
  memcpy(data, &motor220, sizeof(motor220));
  data += sizeof(motor220);
  
  memcpy(data, &motor340, sizeof(motor340));
  data += sizeof(motor340);
  
  memcpy(data, &accel, sizeof(accel));
  data += sizeof(accel);
  
  memcpy(data, &mag, sizeof(mag));
  data += sizeof(mag);
  
  memcpy(data, &rotation, sizeof(rotation));
  data += sizeof(rotation);
  
  memcpy(data, &ir_distance, sizeof(ir_distance));
  data += sizeof(ir_distance);
  
  memcpy(data, &ir_light, sizeof(ir_light));
  data += sizeof(ir_light);
  
  memcpy(data, &light, sizeof(light));
  data += sizeof(light);
  
  memcpy(data, &thermal, sizeof(thermal));
  data += sizeof(thermal);
  
  memcpy(data, &bump, sizeof(bump));
  data += sizeof(bump);
  
  memcpy(data, &error_flags, sizeof(error_flags));
  data += sizeof(error_flags);
  
  memcpy(data, &cycle_time, sizeof(cycle_time));
  data += sizeof(cycle_time);

}

void CritterStateDrop::readArray(void *d) {

  char *data = (char *)d;
  
  memcpy(&power_source, data, sizeof(power_source));
  data += sizeof(power_source);
  
  memcpy(&bus_voltage, data, sizeof(bus_voltage));
  data += sizeof(bus_voltage);
  
  memcpy(&batv40, data, sizeof(batv40));
  data += sizeof(batv40);
  
  memcpy(&batv160, data, sizeof(batv160));
  data += sizeof(batv160);
  
  memcpy(&batv280, data, sizeof(batv280));
  data += sizeof(batv280);
  
  memcpy(&motor100, data, sizeof(motor100));
  data += sizeof(motor100);
  
  memcpy(&motor220, data, sizeof(motor220));
  data += sizeof(motor220);
  
  memcpy(&motor340, data, sizeof(motor340));
  data += sizeof(motor340);
  
  memcpy(&accel, data, sizeof(accel));
  data += sizeof(accel);
  
  memcpy(&mag, data, sizeof(mag));
  data += sizeof(mag);
  
  memcpy(&rotation, data, sizeof(rotation));
  data += sizeof(rotation);
  
  memcpy(&ir_distance, data, sizeof(ir_distance));
  data += sizeof(ir_distance);
  
  memcpy(&ir_light, data, sizeof(ir_light));
  data += sizeof(ir_light);
  
  memcpy(&light, data, sizeof(light));
  data += sizeof(light);
  
  memcpy(&thermal, data, sizeof(thermal));
  data += sizeof(thermal);
  
  memcpy(&bump, data, sizeof(bump));
  data += sizeof(bump);
  
  memcpy(&error_flags, data, sizeof(error_flags));
  data += sizeof(error_flags);
  
  memcpy(&cycle_time, data, sizeof(cycle_time));
  data += sizeof(cycle_time);

}


string CritterStateDrop::toString(string offset) {

  string value;
  char buf[100];

  value += "\nPower Source: ";
  switch(power_source) {
    case SHORE:
      value += "SHORE\n";
      break;
    case BAT40:
      value += "BAT40\n";
      break;
    case BAT160:
      value += "BAT160\n";
      break;
    case BAT280:
      value += "BAT280\n";
      break;
    default:
      value += "ERROR?\n";
      break;
  }
  sprintf(buf, "System Voltage: %u\n", bus_voltage);
  value += buf;
  sprintf(buf, "Battery 40 Voltage: %u\n", batv40);
  value += buf;
  sprintf(buf, "Battery 160 Voltage: %u\n", batv160);
  value += buf;
  sprintf(buf, "Battery 280 Voltage: %u\n\n", batv280);
  value += buf;
  sprintf(buf, "Motor100: %d, %u, %u\n", motor100.velocity,
          motor100.current, motor100.temp);
  value += buf;
  sprintf(buf, "Motor220: %d, %u, %u\n", motor220.velocity,
          motor220.current, motor220.temp);
  value += buf;
  sprintf(buf, "Motor340: %d, %u, %u\n", motor340.velocity,
          motor340.current, motor340.temp);
  value += buf;
  sprintf(buf, "\nAccelerometer: %d %d %d\n", accel.x, accel.y, accel.z);
  value += buf;
  sprintf(buf, "Gyroscope: %d\n", rotation);
  value += buf;
  sprintf(buf, "Magnetometer: %d %d %d\n", mag.x, mag.y, mag.z);
  value += buf;
  sprintf(buf, "IR Distance: %u %u %u %u %u %u %u %u %u %u\n", ir_distance[0], 
    ir_distance[1], ir_distance[2], ir_distance[3], ir_distance[4], 
    ir_distance[5], ir_distance[6], ir_distance[7], ir_distance[8],
    ir_distance[9]);
  value += buf;
  sprintf(buf, "Light Sensors: %u %u %u %u\n", light[0], light[1],
      light[2], light[3]);
  value += buf;
  sprintf(buf, "\nError Codes: %08x\n", error_flags);
  value += buf;
  sprintf(buf, "CPU \%: %u\n", cycle_time);
  value += buf;


  return offset + value;
}
