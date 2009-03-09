#ifndef CritterDriver_debug_H
#define CritterDriver_debug_H

#define BAUDRATE B115200
#define DEVICE "/dev/ttyS0"

#include <termios.h>

#include "Component.h"
#include "BitMask.h"
#include "ErrorMessage.h"
#include "CritterControlDrop.h"
#include "CritterStateDrop.h"

using namespace std;

class CritterDriver_debug : public Component {
  private:
    USeconds lastPost, postWait; //postTimer commandWait
    RiverRead controlId;
    RiverWrite stateId;

    CritterControlDrop* controlDrop;
    CritterStateDrop stateDrop;

    bool newData;

    struct termios oldterm;
    int fid;
    string device;
    int port;

  public:
    
    CritterDriver_debug(DataLake *lake, ComponentConfig &conf, string &name);
    virtual ~CritterDriver_debug();

    virtual int init(USeconds &wokeAt);
    virtual int think(USeconds &wokeAt);
    virtual int sense(USeconds &wokeAt);
    virtual int act(USeconds &wokeAt);

    void cleanup();

 private:
    void initport();
    void closeport();
    void readPacket();
    
};

#endif
