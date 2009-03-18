#include "ImageDrop.h"

ImageDrop::ImageDrop() {
  seqNum = 0;
}

ImageDrop::~ImageDrop() {
  if(img)
    free(img);
}

int ImageDrop::getSize() {
  return sizeof(width) + sizeof(height) + sizeof(seqNum) +
    (img ? width * height : 0);
}

void ImageDrop::writeArray(void *data) {

  int i = 0;
  char *dest = (char *)data;
  memcpy(dest + i, &seqNum, sizeof(seqNum));  i += sizeof(seqNum);
  memcpy(dest + i, &height, sizeof(height));  i += sizeof(height);
  memcpy(dest + i, &width, sizeof(width));    i += sizeof(width);
  if(img != NULL)
    memcpy(dest + i, img, width * height);
}

void ImageDrop::readArray(void *data) {
  
  int i = 0;
  char *dest = (char *)data;
  memcpy(&seqNum, dest + i, sizeof(seqNum));  i += sizeof(seqNum);
  memcpy(&height, dest + i, sizeof(height));  i += sizeof(height);
  memcpy(&width, dest + i, sizeof(width));    i += sizeof(width);
  if(img != NULL)
    memcpy(img, dest + i, width * height);
}

int ImageDrop::setImage( unsigned char *src ) {

  if(img)
    free(img);
  img = (unsigned char *)malloc(width * height);
  if(!img) {
    fprintf(stderr,"Error allocating space!\n");
    return -1;
  }
  memcpy(img, src, width * height);
  return 0;
}

string ImageDrop::toString(string offset) {
  
  unsigned char val;
  int print_height;
  int wScale, hScale;
  char buf[100];
  string out = offset;
  out += "-----------------------------------------\n";
  out += offset;
  if(img) {
    wScale = width / IPRINT_WIDTH;
    print_height = height / wScale / 2;
    hScale = height / print_height;
    for( int i = 0; i < print_height; i++ ) {
      for( int j = 0; j < IPRINT_WIDTH; j++ ) {
        val = *(img + width * i * hScale + j * wScale);
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
