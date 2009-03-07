#include "RewardGenerator.h"
#include "ErrorMessage.h"

/**
 * Constructor for RewardGenerator.  Sets up internal variables and starts reading data from
 * the rivers
 */
RewardGenerator::RewardGenerator(DataLake *lake, 
                                   ComponentConfig &config, 
                                   string &name) : 
                                   Component(lake, config, name) {
	fid = -1;
	stateInput = lake->readyReading("CritterStateDrop");
	rewardOutput = lake->readyWriting("CritterRewardDrop");
	thinks = 0;
}

/**
 * Destructor for RewardGenerator.  Finished reading from rivers.
 */
RewardGenerator::~RewardGenerator() {
	lake -> stopReading(stateInput);
  lake -> stopWriting(rewardOutput);
}

/*
 * This function will read in data from the xml configuration file.
 * Probably you will never need to use this.
 */
int RewardGenerator::loadConfig(ComponentConfig *config) {

  return 1; 
}

/**
 * Initialize Reward related things here
 */
int RewardGenerator::init(USeconds &wokeAt) {

  //  Initialize agent-related foo here
  //

  return 1;  
}

/**
 */
int RewardGenerator::think(USeconds &wokeAt) {
	
	// Read in new data from the world.
  stateDrop = ((CritterStateDrop*)lake->readHead(stateInput));
  lake->doneRead(stateInput);

  // @TODO: This doesn't quite work and should be fixed.
	if(stateDrop) {

	  update();

		(*(CritterRewardDrop*)lake->startWriteHead(rewardOutput)) = rewardDrop;
		lake ->doneWriteHead(rewardOutput);
    
    thinks++;
	}
  return 1;
}

void RewardGenerator::update( void ) {

  // Check the state information
  if( stateDrop->rotation > 10 ) {
    // I'm turning left.
  }
  else if( stateDrop->rotation < -10 ) {
    // I'm turning right.
  }

  // No cookie for you.  Bad robot.
  rewardDrop.reward = 0;
  
  for(int i = 0; i < 32; i++ ) {
    if(stateDrop->bump[i] != 0)
      rewardDrop.reward = 1;
  }

  return;
}
