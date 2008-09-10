#ifndef CritterViz_H
#define CritterViz_H

#include "Component.h"
#include "CritterStateDrop.h"
#include "JoystickDrop.h"

/**
 * @todo: comment me
 */
class CritterViz : public Component {
  public:
    CritterViz(DataLake *lake, ComponentConfig &config, string &name);
    virtual ~CritterViz();

    //for reloading config when it changes
    virtual int loadConfig(ComponentConfig *config) ;

    //initial the device. does not block, will be called multiple times
    //if nessecary. return 0 if not done, negative on error, and positive when done
    virtual int init(USeconds &wokeAt);

    //read data, transform data, write new data
    virtual int think(USeconds &wokeAt);


 private:
		RiverRead critterOutput;
		int thinks;
		bool newData;
		USeconds postWait, lastPost;
};

#endif
