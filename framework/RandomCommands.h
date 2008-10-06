#ifndef RandomCommands_H
#define RandomCommands_H

#include "Component.h"
#include "JoystickDrop.h"

/**
  * RandomCommands is a component that periodically sends random Robot
  * Commands (meant to replace a joystick for simulation purposes). Don't use 
  * with the real robot!
  *
  * Author: Marc G. Bellemare
  * Blatantly copied from CritterViz.{h,cpp}
  */
class RandomCommands : public Component {
  public:
    RandomCommands(DataLake *lake, ComponentConfig &config, string &name);
    virtual ~RandomCommands();

    //for reloading config when it changes
    virtual int loadConfig(ComponentConfig *config) ;

    //initial the device. does not block, will be called multiple times
    //if nessecary. return 0 if not done, negative on error, and positive when done
    virtual int init(USeconds &wokeAt);

    //read data, transform data, write new data
    virtual int think(USeconds &wokeAt);


 private:
		RiverWrite commandWrite;
		int thinks;
		bool newData;
		USeconds postWait, lastPost;
};

#endif
