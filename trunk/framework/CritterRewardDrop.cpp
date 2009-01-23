#include "CritterRewardDrop.h"


string CritterRewardDrop::name = "CritterRewardDrop";

CritterRewardDrop::CritterRewardDrop() {

}

CritterRewardDrop::~CritterRewardDrop() {

}

int CritterRewardDrop::getSize() {
  return sizeof(reward); 
}

void CritterRewardDrop::writeArray(void *d) {

  int i = 0;
  char *data = (char *)d;
  memcpy(data + i, &reward, sizeof(reward)); i += sizeof(reward);
}

void CritterRewardDrop::readArray(void *d) {
  
  int i = 0;
  char *data = (char *)d;
  memcpy(&reward, data + i, sizeof(reward)); i += sizeof(reward);
}

string CritterRewardDrop::toString(string offset) {

  string value;
  char buf[100];

  sprintf (buf, "Reward: %g\n", reward);
  value = buf;

  return offset + value; 
}

string CritterRewardDrop::getName() {
  return CritterRewardDrop::name;
}
