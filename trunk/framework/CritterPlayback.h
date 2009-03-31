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
    virtual int think(USeconds &wokeAt);
    
    virtual BitMask type();
    void cleanup();


 private:
    bool paused;
    int scale;
    int readLog(void);
    FILE* openFile(string fileName);
    void closeFile(void);
    int readConfig(ComponentConfig *config);
    string log_file; 
    /*
     * Start playback if it is puased
     */
    void play(USeconds now);
    
    /*
     * Pause playback of logfile
     */
    void pause(void);
    
    /*
     * Change the speed at which the logfile is played back.
     * = 1, real time, 
     * > 1, multiplier
     */
    void setSpeed(int speed, USeconds now);
};

#endif
