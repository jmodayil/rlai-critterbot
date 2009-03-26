#ifndef ColorImageDrop_H
#define ColorImageDrop_H

#include "Drop.h"

#define PRINT_WIDTH   80

using namespace std;

/**
 * A template DataDrop
 */
class ColorImageDrop : public DataDrop {
  public:
    ColorImageDrop();
    virtual ~ColorImageDrop();

    virtual int getSize();
    virtual void writeArray(void *data);
    virtual void readArray(void *data);
    virtual string toString(string offset = "");
    int width, height;
    long seqNum;
    int setImage( unsigned char *src );
    unsigned char *img;

  private:
};

#endif
