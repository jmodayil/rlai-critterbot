#ifndef SIMULATOR_ROBOT_INTERFACE_H
#define SIMULATOR_ROBOT_INTERFACE_H

#include <string>

#include "Socket.h"
#include "Component.h"
#include "CritterDrop.h"

using namespace std;

#define MAX_ROBOT_INTERFACE_DATA_LENGTH 4096

class SimulatorRobotInterfaceProc : public SocketProtocol {
  private:
    char readData[MAX_ROBOT_INTERFACE_DATA_LENGTH];
    char writeData[MAX_ROBOT_INTERFACE_DATA_LENGTH];
    int writeSize;
    char * writePtr;
    char * readPtr, * unreadDataPtr;

    int readConfig();

    RiverRead stateRead;
    RiverRead controlRead;
    RiverRead rewardRead;
    RiverWrite stateWrite;
    RiverWrite controlWrite;
    RiverWrite rewardWrite;

  protected:
    void clearWriteData();
    void clearReadData();

  public:
    SimulatorRobotInterfaceProc(DataLake *lake, 
      ComponentConfig &config, string name);
    
    virtual int init(USeconds &wokeAt);
    virtual int reloadConfig(ComponentConfig *conf);
    virtual char* readBuffer(int &maxSize);
    virtual int processRead(char *buffer, int size, USeconds &wokeAt);
    virtual char* writeBuffer(int &size, USeconds &wokeAt);
    virtual Socket* createClient(DataLake *lake, ComponentConfig &config,
                                 string name);

    virtual int act(USeconds & wokeAt);

    // Additional methods specific to the SimulatorRobotInterface
    void writeDrop(CritterDrop * drop);
    // Attempts to read a drop from the read buffer (socket) and send it into 
    //  the Disco world
    int processDrop();
};

//! Socket that sends strings
/*!
*/
class SimulatorRobotInterface : public Socket {
  private:
    USeconds wait;

  protected:
    SocketProtocol * myProtocol;

  public:
    SimulatorRobotInterface(DataLake *lake, 
      ComponentConfig &config, string name);
    
    virtual int act(USeconds & wokeAt);
};

#endif
