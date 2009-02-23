#include "RandomCommands.h"
#include "CritterControlDrop.h"

#include <stdlib.h>

RandomCommands::RandomCommands(DataLake *lake, 
                                   ComponentConfig &config, 
                                   string &name) : 
                                   Component(lake, config, name) {
	//	newData = false;
	thinkWait = 1000000;
	thinkTime.setAsNow();
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
  randomState = 0;

  return 1; 
}

int RandomCommands::think(USeconds &wokeAt) {

  if (thinkTime <= wokeAt)
  {
    CritterControlDrop * drop = 
      (CritterControlDrop*)lake->startWriteHead(commandWrite);

    drop->motor_mode = CritterControlDrop::XYTHETA_SPACE;
    drop->m100_vel = drop->m220_vel = drop->m340_vel = 0;
    drop->theta_vel = randomState; // Very slow spin for now
    drop->x_vel = (int)((random() * 1.0) / (RAND_MAX + 1.0) * 50.0 - 25.0);
    drop->y_vel = (int)((random() * 1.0) / (RAND_MAX + 1.0) * 50.0 - 25.0);

    drop->led_mode = CritterControlDrop::NONE;

    lake->doneWriteHead(commandWrite);

    randomState = randomState + 1;
    if (randomState > 10) randomState = -10;
 
    thinkTime = thinkWait + wokeAt;
  }

  return 1;
} 
