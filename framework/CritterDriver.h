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

#define SER_HEADER1 0xDE
#define SER_HEADER2 0xAD
#define SER_HEADER3 0xBE
#define SER_HEADER4 0xEF

using namespace std;

class CritterDriver : public Component {
  private:
    USeconds lastPost, postWait; //postTimer commandWait
    RiverRead controlId;
    RiverWrite stateId;

    CritterControlDrop* controlDrop;
    CritterStateDrop stateDrop;

    bool newData;

    int acts;
    
    struct termios oldterm;
    int fid;
    string device;
    int port;

  public:
    
    CritterDriver(DataLake *lake, ComponentConfig &conf, string &name);
    virtual ~CritterDriver();

    virtual int getFID();
    virtual int init(USeconds &wokeAt);
    //virtual int think(USeconds &wokeAt);
    virtual int sense(USeconds &wokeAt);
    virtual int act(USeconds &wokeAt);

    //virtual int loadConfig(ComponentConfig *conf);
    int readConfig(ComponentConfig *conf, string &device, int &postWait);
    void cleanup();

 private:
    void initport();
    void closeport();
    void readPacket();
    
};

#endif
