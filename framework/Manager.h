#ifndef _H
#define Manager_H

#include "Component.h"
#include "CritterControlDrop.h"
#include "JoystickDrop.h"

/**
 * @todo: comment me
 */
class Manager : public Component {
  public:
    Manager(DataLake *lake, ComponentConfig &config, string &name);
    virtual ~Manager();

    //for reloading config when it changes
    virtual int loadConfig(ComponentConfig *config) ;

    //initial the device. does not block, will be called multiple times
    //if nessecary. return 0 if not done, negative on error, and positive when done
    virtual int init(USeconds &wokeAt);

    //read data, transform data, write new data
    virtual int think(USeconds &wokeAt);


 private:
		RiverWrite controlOutput;
		RiverRead joystickInput;
};

#endif
