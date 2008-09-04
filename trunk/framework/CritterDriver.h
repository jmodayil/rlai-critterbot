#ifndef CritterDriver_H
#define CritterDriver_H

#define BAUDRATE B115200
#define DEVICE "/dev/ttyS0"

#include <termios.h>

#include "Component.h"
#include "BitMask.h"
#include "ErrorMessage.h"
#include "CritterControlDrop.h"
#include "CritterStateDrop.h"

using namespace std;

class CritterDriver : public Component {
  private:
    USeconds lastPost, postWait; //postTimer commandWait
    RiverRead controlId;
    RiverWrite stateId;

    CritterControlDrop* controlDrop;
    CritterStateDrop stateDrop;

    bool newData;

    int thinks;
    
    struct termios oldterm;
    int fid;
    string device;
    int port;

  public:
    
    CritterDriver(DataLake *lake, ComponentConfig &conf, string &name);
    virtual ~CritterDriver();

    virtual int getFID();
    virtual int init(USeconds &wokeAt);
    virtual int think(USeconds &wokeAt);
    virtual int sense(USeconds &wokeAt);
    virtual int loadConfig(ComponentConfig *conf);
    int readConfig(ComponentConfig *conf, string &device, int &postWait);
    void cleanup();

 private:
    void initport();
    void closeport();
    void readPacket();
    
};

#endif
