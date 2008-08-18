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
  

	//config comes from the parent
	if(this->config == *config) {

  }
  //config is new
  else {
    this->config = *config;
  }

  return 1; 
}

int Manager::init(USeconds &wokeAt) {
  return 1; 
	
	joystickInput = lake->readyReading(JoystickDrop);
	controlOutput = lake->readyWriting(ControlDrop);

}

int Manager::think(USeconds &wokeAt) {
	ControlDrop* drop;
	// read in the Joystick Control Drop
	// translate the joystick controls into robot commands
	// write the robot commands to the drop
	
	drop = ((ControlDrop*)lake->startWriteHead(controlOutput));

	lake ->doneWriteHead(controlOutput);
	thinks++;
  return 1;
}
