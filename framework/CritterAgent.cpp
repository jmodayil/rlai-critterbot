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

  int x,y,t;
  controlDrop.motor_mode = CritterControlDrop::XYTHETA_SPACE;
  
  x = 20;
  y = 0;
  t = 0;
  if( stateDrop->ir_distance[0] > 65 ) {
   x -= (stateDrop->ir_distance[0] - 65) / 10;
  }
  if( stateDrop->ir_distance[1] > 65 ) {
   x -= (stateDrop->ir_distance[1] - 65) / 20;
   t += (stateDrop->ir_distance[1] - 65) / 20; 
  }
  if( stateDrop->ir_distance[6] > 65 ) {
   x -= (stateDrop->ir_distance[6] - 65) / 20;
   t -= (stateDrop->ir_distance[6] - 65) / 20; 
  } 
  if( stateDrop->ir_distance[2] > 65 ) {
   t += (stateDrop->ir_distance[2] - 65) / 20;
   y -= (stateDrop->ir_distance[2] - 65) / 20; 
  } 
  if( stateDrop->ir_distance[5] > 65 ) {
   t -= (stateDrop->ir_distance[5] - 65) / 20;
   y += (stateDrop->ir_distance[5] - 65) / 20; 
  } 
  if( stateDrop->ir_distance[7] > 60 ) {
   y -= (stateDrop->ir_distance[7] - 60) / 5; 
  } 
  if( stateDrop->ir_distance[9] > 65 ) {
   y += (stateDrop->ir_distance[9] - 65) / 10; 
  } 
  controlDrop.x_vel = x;
  controlDrop.y_vel = y;
  controlDrop.theta_vel = t;
  // Send a new action.

  return;
}
