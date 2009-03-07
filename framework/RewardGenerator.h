#ifndef RewardGenerator_H
#define RewardGenerator_H

#include "Component.h"
#include "CritterRewardDrop.h"
#include "CritterControlDrop.h"
#include "CritterStateDrop.h"

/**
 * 
 */
class RewardGenerator : public Component {
  public:
    RewardGenerator(DataLake *lake, ComponentConfig &config, string &name);
    virtual ~RewardGenerator();

    //for reloading config when it changes
    virtual int loadConfig(ComponentConfig *config) ;

    //initial the device. does not block, will be called multiple times
    //if nessecary. return 0 if not done, negative on error, and positive when done
    virtual int init(USeconds &wokeAt);

    //read data, transform data, write new data
    virtual int think(USeconds &wokeAt);


 private:

    void update( void );

		RiverRead controlInput;
		RiverRead stateInput;
    RiverWrite rewardOutput;

		CritterRewardDrop rewardDrop;
		CritterStateDrop* stateDrop;

		bool newData;
		int thinks;
		int fid;

		// Agent variables go here...
};

#endif
