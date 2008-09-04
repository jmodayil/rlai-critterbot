#ifndef CritterDriver_H
#define CritterDriver_H

#include "Component.h"
#include "BitMask.h"
#include "ErrorMessage.h"
#include "CritrterControlDrop.h"
#include "CritrterStateDrop.h"

using namespace std;

class CritterDriver : public Component {
  private:
    USeconds lastPost, postWait; //postTimer commandWait
    RiverRead controlId;
    RiverWrite stateId;

    CritterControlDrop* controlDrop;
    CritterStateDrop stateDrop;

    boolean newData;

    int thinks;
    
    int postWait;
    USeconds lastPost;
    int fid;
    string device;
    int port;

  public:
    
    CritterDriver();
    virtual ~CritterDriver();

    //returns the fid of the io device. If no fid exists,
    //will return fileno(stdout). stdout is never ready for reading.
    //return fileno(stdout) means poll me, i'm not event driven.
    virtual int getFID();

    //reads from the device to DataLake. if the fid exists, receive() will
    //only be called when data is present
    //if the device wishes to be closed, return a negative number
    virtual int sense(USeconds &wokeAt);

    //initial the device. does not block, will be called multiple times
    //if nessecary. return 0 if not done.
    virtual int init(USeconds &wokeAt);

    virtual int think(USeconds &wokeAt);
    virtual int sense(USeconds &wokeAt);

    virtual int loadConfig(ComponentConfig *conf);

    int readConfig(ComponentConfig *conf, string &device, int &postWait);

    //cleanup
    void cleanup();

 private:
    void initport(int port);
    void closeport(int port);
    void readPacket();
    
};

#endif
