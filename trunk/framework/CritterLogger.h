#ifndef CritterLogger_H
#define CritterLogger_H

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

class CritterLogger : public Component {
  private:
    RiverRead controlId;
    RiverRead logTagId;
    RiverRead stateId;
    RiverRead rewardId;

    CritterLogTagDrop* logTagDrop;
    CritterControlDrop* controlDrop;
    CritterStateDrop* stateDrop;
    CritterRewardDrop* rewardDrop;

    USeconds last_log; 
    FILE *log;
  
  public:
    
    CritterLogger(DataLake *lake, ComponentConfig &conf, string &name);
    virtual ~CritterLogger();

    virtual int getFID();
    virtual int init(USeconds &wokeAt);
    virtual int act(USeconds &wokeAt);

    void cleanup();

 private:
    void logData(CritterStateDrop *stateDrop, USeconds *theTime); 
    FILE* rotate_log( FILE *log, USeconds *now );
    int readConfig(ComponentConfig *config);
    string log_path; 
    string file_timestamp;
};

#endif
