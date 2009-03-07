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
  return 1; 
}

/**
 * Initialize Agent related things here
 */
int CritterAgent::init(USeconds &wokeAt) {
  
  // @TODO: Default values etc...
  actionDir = 1;

  // You will want to always return 1.
  return 1; 
}

int CritterAgent::think(USeconds &wokeAt) {
	
	// Read in new data from the world.
  stateDrop = ((CritterStateDrop*)lake->readHead(stateInput));
  rewardDrop = ((CritterRewardDrop*)lake->readHead(rewardInput));

  if (rewardDrop) {
    // Check the reward
    if( rewardDrop->reward != 0) {
      actionDir = -actionDir;
    }
  }

    
	if(stateDrop) {

	  update();

		(*(CritterControlDrop*)lake->startWriteHead(controlOutput)) = controlDrop;
		lake ->doneWriteHead(controlOutput);
    
    thinks++;
	}
  
  lake->doneRead(stateInput);
  lake->doneRead(rewardInput);
  return 1;
}

/**
 * This is where the brains of the Agent live.
 * 
 * update gets called at regular intervals, and you can do stuff here.
 */
void CritterAgent::update( void ) {

  // Check the state information
  if( stateDrop->rotation > 10 ) {
    // I'm turning left.
  }
  else if( stateDrop->rotation < -10 ) {
    // I'm turning right.
  }

  // Send a new action.
  controlDrop.motor_mode = CritterControlDrop::XYTHETA_SPACE;
  controlDrop.x_vel = 100 * actionDir;
  controlDrop.y_vel = 0;
  controlDrop.theta_vel = 0;

  return;
}
