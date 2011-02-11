#include <stdio.h>
#include "USeconds.h"

USeconds::USeconds() {
  time.tv_usec = 0;
  time.tv_sec = 0;
}

USeconds::USeconds(const USeconds &copy) {
  time = copy.time;
}

USeconds::USeconds(unsigned int milliseconds) {
  time.tv_usec = milliseconds % ONE_MILLION;
  time.tv_sec = milliseconds / ONE_MILLION;
}

int USeconds::getSize() const {
  return sizeof(int)*2;
}

void USeconds::writeArray(void *data) const {
  unsigned int *d = (unsigned int*)data;
  d[0] = time.tv_sec;
  d[1] = time.tv_usec;
}

void USeconds::readArray(void *data) {
  unsigned int *d = (unsigned int*)data;
  time.tv_sec = d[0];
  time.tv_usec = d[1];
}

void USeconds::setAsNow() {
  gettimeofday(&time, NULL);
}
