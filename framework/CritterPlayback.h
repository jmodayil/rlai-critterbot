#ifndef CritterPlayback_H
#define CritterPlayback_H

#include "Component.h"
#include "BitMask.h"
#include "ErrorMessage.h"
#include "CritterControlDrop.h"
#include "CritterStateDrop.h"
#include "CritterRewardDrop.h"
#include "CritterLogTagDrop.h"
#include "USeconds.h"

#define LOG_INTERVAL 5 // in minutes
#define LOG_EXTENSION ".crtrlog"

using namespace std;

class CritterPlayback : public Component {
  private:
    RiverWrite logTagId;
    RiverWrite stateId;
    RiverWrite rewardId;

    CritterLogTagDrop logTagDrop;
    CritterStateDrop stateDrop;
    CritterRewardDrop rewardDrop;

    USeconds startTime; 
    USeconds currentTime;
    USeconds wallStartTime;
    FILE *log;
  
  public:
    
    CritterPlayback(DataLake *lake, ComponentConfig &conf, string &name);
    virtual ~CritterPlayback();

    virtual int getFID();
    virtual int init(USeconds &wokeAt);
    virtual int act(USeconds &wokeAt);

    void cleanup();

 private:
    int readLog(void);
    FILE* openFile(string fileName);
    int readConfig(ComponentConfig *config);
    string log_file; 
};

#endif
