#ifndef CRITTER_DROP_H
#define CRITTER_DROP_H

#include "Drop.h"

/** Defines an abstract class, CritterDrop, which contains additional methods necessary for
  *  our drops.
  */
class CritterDrop : public DataDrop {
  
  public:
    virtual string getName() = 0;
};

#endif

