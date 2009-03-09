#ifndef Photovore_H
#define Photovore_H

#include "Component.h"
#include "CritterControlDrop.h"
#include "CritterStateDrop.h"

/**
 * @todo: comment me
 */
class Photovore : public Component {
  public:
    Photovore(DataLake *lake, ComponentConfig &config, string &name);
    virtual ~Photovore();

    //for reloading config when it changes
    virtual int loadConfig(ComponentConfig *config) ;

    //initial the device. does not block, will be called multiple times
    //if nessecary. return 0 if not done, negative on error, and positive when done
    virtual int init(USeconds &wokeAt);

    //read data, transform data, write new data
    virtual int think(USeconds &wokeAt);


 private:
		RiverWrite controlOutput;
		RiverRead stateInput;

		CritterControlDrop controlDrop;
		CritterStateDrop* stateDrop;

		bool newData;

		int fid;

		// control variables
		unsigned int front_last;
    int dir;
};

#endif
