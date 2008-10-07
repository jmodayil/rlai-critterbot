#ifndef SimulatorInterface_H
#define SimulatorInterface_H

#include "Component.h"

/**
  * The simulator interface. This is meant as an interface between DisCo
  *  and the (java-based) robot simulator. It consumes CritterControlDrop's
  *  and produces CritterStateDrop's. It also produces drops containing
  *  information about the simulator's objective state.
  *
  * Author: Marc G. Bellemare
  */
class SimulatorInterface : public Component {
  public:
    SimulatorInterface(DataLake *lake, ComponentConfig &config, string &name);
    virtual ~SimulatorInterface();

    //for reloading config when it changes
    virtual int loadConfig(ComponentConfig *config) ;

    //initial the device. does not block, will be called multiple times
    //if nessecary. return 0 if not done, negative on error, and positive when done
    virtual int init(USeconds &wokeAt);

    //read data, transform data, write new data
    virtual int think(USeconds &wokeAt);


 private:
		RiverWrite stateWrite;
    RiverRead  controlRead;
		int thinks;
		bool newData;
		USeconds postWait, lastPost;

    int robotSocket;
    int objectiveSocket;

    int robotPort;
    int objectivePort; 
};

#endif
