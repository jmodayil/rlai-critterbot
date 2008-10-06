#include "RandomCommands.h"
#include "CritterControlDrop.h"

RandomCommands::RandomCommands(DataLake *lake, 
                                   ComponentConfig &config, 
                                   string &name) : 
                                   Component(lake, config, name) {
	//	newData = false;
	postWait = 1000000;
	lastPost.setAsNow();
}

RandomCommands::~RandomCommands() {
	lake->doneWriteHead(commandWrite);
}

int RandomCommands::loadConfig(ComponentConfig *config) {
  // see FakeLaser.cpp for ideas about how to read the config
	// file once we have some things that need to be configured
  return 1; 
}

int RandomCommands::init(USeconds &wokeAt) {
	commandWrite = lake->readyWriting(CritterControlDrop::name);
  return 1; 
}

int RandomCommands::think(USeconds &wokeAt) {
  CritterControlDrop * drop = 
    (CritterControlDrop*)lake->startWriteHead(commandWrite);

  drop->motor_mode = CritterControlDrop::XYTHETA_SPACE;
  drop->m100_vel = drop->m220_vel = drop->m340_vel = 0;
  drop->theta_vel = 1; // Very slow spin for now
  drop->led_mode = CritterControlDrop::THING1;

  lake->doneWriteHead(commandWrite);

  return 1;
} 
