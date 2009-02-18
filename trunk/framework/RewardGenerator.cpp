#include "RewardGenerator.h"

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
  controlInput = lake->readyReading("CritterControlDrop");
	thinks = 0;
	newData = false;
}

/**
 * Destructor for RewardGenerator.  Finished reading from rivers.
 */
RewardGenerator::~RewardGenerator() {
	lake -> stopReading(stateInput);
  lake -> stopWriting(rewardOutput);
	lake -> stopReading(controlInput);
}


int RewardGenerator::loadConfig(ComponentConfig *config) {
  // see FakeLaser.cpp for ideas about how to read the config
	// file once we have some things that need to be configured
  

  return 1; 
}

/**
 * Initialize Reward related things here
 */
int RewardGenerator::init(USeconds &wokeAt) {
  
  // @TODO: Default values etc...

  // You will want to always return 1.
  return 1; 
}

/**
 */
int RewardGenerator::think(USeconds &wokeAt) {
	
	// Read in new data from the world.
  stateDrop = ((CritterStateDrop*)lake->readHead(stateInput));
  lake->doneRead(stateInput);
  controlDrop = ((CritterControlDrop*)lake->readHead(controlInput));
  lake->doneRead(controlInput);

  // @TODO: This doesn't quite work and should be fixed.
	if(stateDrop && controlDrop) {

	  update();

		(*(CritterRewardDrop*)lake->startWriteHead(rewardOutput)) = rewardDrop;
		lake ->doneWriteHead(rewardOutput);
    
    thinks++;
	}
  return 1;
}

void RewardGenerator::update( void ) {

  // Check the command 
  if( controlDrop->motor_mode == CritterControlDrop::XYTHETA_SPACE) {
    // We're in XYTheta space!  Yay.
  }

  // Check the state information
  if( stateDrop->rotation > 10 ) {
    // I'm turning left.
  }
  else if( stateDrop->rotation < -10 ) {
    // I'm turning right.
  }

  // No cookie for you.  Bad robot.
  rewardDrop.reward = 0;

  return;
}
