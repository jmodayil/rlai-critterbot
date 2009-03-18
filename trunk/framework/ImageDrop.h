#ifndef ImageDrop_H
#define ImageDrop_H

#include "Drop.h"

#define IPRINT_WIDTH   80

using namespace std;

/**
 * A template DataDrop
 */
class ImageDrop : public DataDrop {
  public:
    ImageDrop();
    virtual ~ImageDrop();

    virtual int getSize();
    virtual void writeArray(void *data);
    virtual void readArray(void *data);
    virtual string toString(string offset = "");
    int width, height;
    long seqNum;
    int setImage( unsigned char *src );

  private:
    unsigned char *img;
};

#endif
