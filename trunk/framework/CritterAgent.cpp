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
  controlOutput = lake->readyWriting("CritterControlDrop");
}

/**
 * Destructor for CritterAgent.  Finished reading from rivers.
 */
CritterAgent::~CritterAgent() {
	lake -> stopReading(stateInput);
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

  controlDrop.motor_mode = CritterControlDrop::XYTHETA_SPACE;
  // Check the state information
  if( stateDrop->ir_distance[1] > 75 || stateDrop->ir_distance[2] > 75 ||
      stateDrop->ir_distance[0] > 75) {
    controlDrop.x_vel = 7;
    controlDrop.y_vel = 7;
    controlDrop.theta_vel = 6;
  }
  else {
    controlDrop.x_vel = 10;
    controlDrop.y_vel = 10;
    controlDrop.theta_vel = -1;
  }
  // Send a new action.

  return;
}
