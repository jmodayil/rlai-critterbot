#include "Manager.h"

Manager::Manager(DataLake *lake, 
                                   ComponentConfig &config, 
                                   string &name) : 
                                   Component(lake, config, name) {
	fid = -1;
	joystickInput = lake->readyReading("JoystickDrop");
	controlOutput = lake->readyWriting("CritterControlDrop");
	thinks = 0;
	speed = 25;
	newData = false;
	
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
	// why would I do things in init rather than the constructor?
  return 1; 
}

int Manager::think(USeconds &wokeAt) {
	//do we need to wait and think and all that?

	// read in the Joystick Control Drop
	joystickDrop = ((JoystickDrop*)lake->readHead(joystickInput));

	// translate the joystick controls into robot commands
	// write the robot commands to the drop
	if(joystickDrop) {

		//translate commands
		// buttons 1-4 control the percentage speed
		// button 7 toggles through led settings
		if(joystickDrop->buttons.somethingSet()) {
			if(joystickDrop->buttons.isSet(0))
				speed = 25;
			else if(joystickDrop->buttons.isSet(1))
				speed = 50;
			else if(joystickDrop->buttons.isSet(2))
				speed = 75;
			else if(joystickDrop->buttons.isSet(3))
				speed = 100;
			else if(joystickDrop->buttons.isSet(6))
			{
				// how do we toggle through led settings?
			}
		}
		// the right analog button moves linearly, the left analog button
		// controls the turning
		// these are set to a valid between -100 and 100 ?
		controlDrop.motor_mode = CritterControlDrop::XYTHETA_SPACE;
		controlDrop.x_vel = -(int)speed*(joystickDrop->rightAnalogY)/JOY_AXIS_MAX;
		controlDrop.y_vel = -(int)speed*(joystickDrop->rightAnalogX)/JOY_AXIS_MAX;
		controlDrop.theta_vel = (int)speed*(joystickDrop->leftAnalogX)/JOY_AXIS_MAX;
		
		//copying James
		(*(CritterControlDrop*)lake->startWriteHead(controlOutput)) = controlDrop;
		lake ->doneWriteHead(controlOutput);
	}
	lake ->doneRead(joystickInput);
	thinks++;
  return 1;
}
