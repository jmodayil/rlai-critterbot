#include "CritterLogTagDrop.h"

string CritterLogTagDrop::name = "CritterLogTagDrop";

CritterLogTagDrop::CritterLogTagDrop() {

}

CritterLogTagDrop::~CritterLogTagDrop() {

}

int CritterLogTagDrop::getSize() {
  return tagInfo.size() + 1;
}

void CritterLogTagDrop::writeArray(void *d) {

  strcpy((char *)d, tagInfo.c_str());
}

void CritterLogTagDrop::readArray(void *d) {
  
  tagInfo = (char *)d;
}

string CritterLogTagDrop::toString(string offset) {

  return offset + tagInfo; 
}
