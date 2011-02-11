#include <string.h>
#include "CritterStateDrop.h"

std::string CritterStateDrop::name = "CritterStateDrop";

CritterStateDrop::CritterStateDrop() {
  data_source = ROBOT;
  power_source = SHORE;
  charge_state = UNPLUGGED;
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
  motor100.command = 0;
  motor100.velocity = 0;
  motor100.current = 0;
  motor100.temp = 0;
  motor220.command = 0;
  motor220.velocity = 0;
  motor220.current = 0;
  motor220.temp = 0;
  motor340.command = 0;
  motor340.velocity = 0;
  motor340.current = 0;
  motor340.temp = 0;
  cycle_time = 0;
  monitor_state = 0;
}

CritterStateDrop::~CritterStateDrop() {
}

int CritterStateDrop::getDropSize() const {
	return 4 + name.length() + 4 + getDataSize();
}

int CritterStateDrop::getDataSize() const {
  int size;
  size = time.getSize();
  size += sizeof(data_source);
  size += sizeof(power_source);
  size += sizeof(charge_state);
  size += sizeof(bus_voltage);
  size += sizeof(batv40);
  size += sizeof(batv160);
  size += sizeof(batv280);
  size += sizeof(motor100);
  size += sizeof(motor220);
  size += sizeof(motor340);
  size += sizeof(accel);
  size += sizeof(mag);
  size += sizeof(rotation);
  size += sizeof(ir_distance);
  size += sizeof(ir_light);
  size += sizeof(light);
  size += sizeof(thermal);
  size += sizeof(bump);
  size += sizeof(error_flags);
  size += sizeof(cycle_time);
  size += sizeof(monitor_state);
  return size;
}

void CritterStateDrop::writeArray(void *d) const {
  char *data = (char *)d;

  int nameLength = name.length();
  memcpy(data, &nameLength, sizeof(nameLength));
  data += sizeof(nameLength);

  memcpy(data, name.c_str(), name.length());
  data += name.length();

  int dataSize = getDataSize();
  memcpy(data, &dataSize, sizeof(dataSize));
  data += sizeof(dataSize);


  time.writeArray(data);
  data += time.getSize();

  memcpy(data, &data_source, sizeof(data_source));
  data += sizeof(data_source);

  memcpy(data, &power_source, sizeof(power_source));
  data += sizeof(power_source);

  memcpy(data, &charge_state, sizeof(charge_state));
  data += sizeof(charge_state);

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

  memcpy(data, &monitor_state, sizeof(monitor_state));
  data += sizeof(monitor_state);
}
