#ifndef CritterAgent_H
#define CritterAgent_H

#include "Component.h"
#include "CritterRewardDrop.h"
#include "CritterControlDrop.h"
#include "CritterStateDrop.h"

/**
 * 
 */
class CritterAgent : public Component {
  public:
    CritterAgent(DataLake *lake, ComponentConfig &config, string &name);
    virtual ~CritterAgent();

    //for reloading config when it changes
    virtual int loadConfig(ComponentConfig *config) ;

    //initial the device. does not block, will be called multiple times
    //if nessecary. return 0 if not done, negative on error, and positive when done
    virtual int init(USeconds &wokeAt);

    //read data, transform data, write new data
    virtual int think(USeconds &wokeAt);


 private:

    void update( void );

		RiverWrite controlOutput;
		RiverRead stateInput;
    RiverRead rewardInput;

		CritterRewardDrop* rewardDrop;
    CritterControlDrop controlDrop;
		CritterStateDrop* stateDrop;

		bool newData;
		int thinks;
		int fid;

		// Agent variables go here...
    //
};

#endif
