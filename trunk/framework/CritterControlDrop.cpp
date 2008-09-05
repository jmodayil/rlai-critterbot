#include "CritterControlDrop.h"

string CritterControlDrop::name = "CritterControlDrop";

CritterControlDrop::CritterControlDrop() {

}

CritterControlDrop::~CritterControlDrop() {

}

int CritterControlDrop::getSize() {
  return sizeof(robot_command);
}

void CritterControlDrop::writeArray(void *data) {
  //memcpy(data, dataPoints, sizeof(dataPoints));
}

void CritterControlDrop::readArray(void *data) {
  //memcpy(dataPoints, data, sizeof(dataPoints));
}

string CritterControlDrop::toString(string offset) {
}

string CritterControlDrop::getName() {
  return name;
}
