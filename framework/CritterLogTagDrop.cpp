#include "CritterLogTagDrop.h"

string CritterLogTagDrop::name = "CritterLogTagDrop";

CritterLogTagDrop::CritterLogTagDrop() {
}

CritterLogTagDrop::~CritterLogTagDrop() {

}

int CritterLogTagDrop::getSize() {
  return tagInfo.size() + 4 + tagName.size() + 4;
}

void CritterLogTagDrop::writeArray(void *d) {

  char *data = (char *)d;
  int len = tagName.length();
  *(int *)data = len;  data += sizeof(int);
  memcpy(data, tagName.c_str(), len);
  data += len;
  len = tagInfo.length(); 
  *(int *)data = len;  data += sizeof(int);
  memcpy(data, tagInfo.c_str(), len);
}

void CritterLogTagDrop::readArray(void *d) {
  
  char *data = (char *)d;
  int len = *((int *)data);
  data += sizeof(int);
  while(len--) tagName += *data++;
  len = *((int *)data);
  data += sizeof(int);
  while(len--) tagInfo += *data++;
}

string CritterLogTagDrop::toString(string offset) {

  return offset + tagName + ": " + tagInfo; 
}
