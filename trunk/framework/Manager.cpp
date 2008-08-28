#include "Manager.h"

Manager::Manager(DataLake *lake, 
                                   ComponentConfig &config, 
                                   string &name) : 
                                   Component(lake, config, name) {

}

Manager::~Manager() {
	//might want to make an array of these and just iterate?
	lake -> stopReading(joystickInput);

	lake -> stopWriting(controlOutput);
}

int Manager::loadConfig(ComponentConfig *config) {
  // see FakeLaser.cpp for ideas about how to read the config
	// file once we have some things that need to be configured
  

  return 1; 
}

int Manager::init(USeconds &wokeAt) {
	joystickInput = lake->readyReading("JoystickDrop");
	controlOutput = lake->readyWriting("ControlDrop");
  return 1; 
}

int Manager::think(USeconds &wokeAt) {
	CritterControlDrop* drop;
	// read in the Joystick Control Drop
	// translate the joystick controls into robot commands
	// write the robot commands to the drop
	
	drop = ((CritterControlDrop*)lake->startWriteHead(controlOutput));

	lake ->doneWriteHead(controlOutput);
	thinks++;
  return 1;
}
