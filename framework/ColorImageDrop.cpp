#include "ColorImageDrop.h"

ColorImageDrop::ColorImageDrop() {
  seqNum = 0;
  img = NULL;
}

ColorImageDrop::~ColorImageDrop() {
  if(img)
    free(img);
}

int ColorImageDrop::getSize() {
  return sizeof(width) + sizeof(height) + sizeof(seqNum) +
    (img ? width * height * 3 : 0);
}

void ColorImageDrop::writeArray(void *data) {

  int i = 0;
  char *dest = (char *)data;
  memcpy(dest + i, &seqNum, sizeof(seqNum));  i += sizeof(seqNum);
  memcpy(dest + i, &height, sizeof(height));  i += sizeof(height);
  memcpy(dest + i, &width, sizeof(width));    i += sizeof(width);
  if(img != NULL)
    memcpy(dest + i, img, width * height * 3);
}

void ColorImageDrop::readArray(void *data) {
  
  int i = 0;
  char *dest = (char *)data;
  memcpy(&seqNum, dest + i, sizeof(seqNum));  i += sizeof(seqNum);
  memcpy(&height, dest + i, sizeof(height));  i += sizeof(height);
  memcpy(&width, dest + i, sizeof(width));    i += sizeof(width);
  if(img != NULL)
    memcpy(img, dest + i, width * height * 3);
}

int ColorImageDrop::setImage( unsigned char *src ) {

  if(img)
    delete[] img;
  img = (unsigned char *)new char[width * height * 3];
  //img = (unsigned char *)malloc(width * height * 3);
  if(!img) {
    fprintf(stderr,"Error allocating space!\n");
    return -1;
  }
  memcpy(img, src, width * height * 3);
  return 0;
}

string ColorImageDrop::toString(string offset) {
  
  unsigned char val;
  int print_height;
  int wScale, hScale;
  char buf[100];
  string out = offset;
  out += "-----------------------------------------\n";
  out += offset;
  if(img) {
    wScale = width / PRINT_WIDTH;
    print_height = height / wScale / 2;
    hScale = height / print_height;
    for( int i = 0; i < print_height; i++ ) {
      for( int j = 0; j < PRINT_WIDTH; j++ ) {
        val = *(img + width * i * hScale * 3 + j * wScale * 3);
        if( val < 50 )
          out += "#";
        else if( val < 100 )
          out += "o";
        else if( val < 150 )
          out += "*";
        else if( val < 200 )
          out += ".";
        else
          out += " ";
      }
    out += "\n";
    out += offset;
    }
  }
  else {
    out += "NO IMAGE\n";
    out += offset;
  }
  out += "-----------------------------------------\n";
  out += offset;
  sprintf(buf, "Sequence num: %d Width: %d Height: %d", 
      seqNum, width, height );
  out += buf;
  return out;
}
