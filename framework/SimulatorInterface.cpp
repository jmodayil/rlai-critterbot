#include "SimulatorInterface.h"
#include "CritterControlDrop.h"
#include "CritterStateDrop.h"

SimulatorInterface::SimulatorInterface(DataLake *lake, 
                                   ComponentConfig &config, 
                                   string &name) : 
                                   Component(lake, config, name) {
	//	newData = false;
	postWait = 1000000;
	lastPost.setAsNow();
}

SimulatorInterface::~SimulatorInterface() {
	lake->doneWriteHead(stateWrite);
  lake->doneRead(controlRead);
}

int SimulatorInterface::loadConfig(ComponentConfig *config) {
  // see FakeLaser.cpp for ideas about how to read the config
	// file once we have some things that need to be configured
  return 1; 
}

int SimulatorInterface::init(USeconds &wokeAt) {
	stateWrite = lake->readyWriting(CritterStateDrop::name);
  controlRead = lake->readyReading(CritterControlDrop::name);

  return 1; 
}

int SimulatorInterface::think(USeconds &wokeAt) {
  // 1. Read in CritterControlDrop, pass it along if available
  // 2. Read from simulator, create CritterStateDrop
  return 1;
} 
