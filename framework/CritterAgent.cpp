#include "CritterAgent.h"

/**
 * Constructor for CritterAgent.  Sets up internal variables and starts reading data from
 * the rivers
 */
CritterAgent::CritterAgent(DataLake *lake, 
                                   ComponentConfig &config, 
                                   string &name) : 
                                   Component(lake, config, name) {
	fid = -1;
	stateInput = lake->readyReading("CritterStateDrop");
	rewardInput = lake->readyReading("CritterRewardDrop");
  controlOutput = lake->readyWriting("CritterControlDrop");
	thinks = 0;
	newData = false;
}

/**
 * Destructor for CritterAgent.  Finished reading from rivers.
 */
CritterAgent::~CritterAgent() {
	lake -> stopReading(stateInput);
  lake -> stopReading(rewardInput);
	lake -> stopWriting(controlOutput);
}


int CritterAgent::loadConfig(ComponentConfig *config) {
  // see FakeLaser.cpp for ideas about how to read the config
	// file once we have some things that need to be configured
  

  return 1; 
}

/**
 * Initialize Agent related things here
 */
int CritterAgent::init(USeconds &wokeAt) {
  
  // @TODO: Default values etc...

  // You will want to always return 1.
  return 1; 
}

/**
 * This is where the brains of the Agent live.
 * 
 * think gets called at regular intervals, and you can do stuff here.
 */
int CritterAgent::think(USeconds &wokeAt) {
	
	// Read in new data from the world.
  stateDrop = ((CritterStateDrop*)lake->readHead(stateInput));
  lake->doneRead(stateInput);
  rewardDrop = ((CritterRewardDrop*)lake->readHead(rewardInput));
  lake->doneRead(rewardInput);

  // @TODO: This doesn't quite work and should be fixed.
	if(stateDrop && rewardDrop) {

	  update();

		(*(CritterControlDrop*)lake->startWriteHead(controlOutput)) = controlDrop;
		lake ->doneWriteHead(controlOutput);
    
    thinks++;
	}
  return 1;
}

void CritterAgent::update( void ) {

  // Check the reward
  if( rewardDrop->reward != 0) {
    // I got a reward!  Yay.
  }

  // Check the state information
  if( stateDrop->rotation > 10 ) {
    // I'm turning left.
  }
  else if( stateDrop->rotation < -10 ) {
    // I'm turning right.
  }

  // Send a new action.
  controlDrop.motor_mode = CritterControlDrop::XYTHETA_SPACE;
  controlDrop.x_vel = 0;
  controlDrop.y_vel = 0;
  controlDrop.theta_vel = 0;

  return;
}
