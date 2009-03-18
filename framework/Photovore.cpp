#include "Photovore.h"

Photovore::Photovore(DataLake *lake, 
                                   ComponentConfig &config, 
                                   string &name) : 
                                   Component(lake, config, name) {
	fid = -1;
	stateInput = lake->readyReading("CritterStateDrop");
	controlOutput = lake->readyWriting("CritterControlDrop");
	thinks = 0;
	newData = false;
  front_last = 0;	
  dir = 1;
}

Photovore::~Photovore() {
	//might want to make an array of these and just iterate?
	lake -> stopReading(stateInput);
	lake -> stopWriting(controlOutput);
}

int Photovore::loadConfig(ComponentConfig *config) {
  // see FakeLaser.cpp for ideas about how to read the config
	// file once we have some things that need to be configured
  

  return 1; 
}

int Photovore::init(USeconds &wokeAt) {
	// why would I do things in init rather than the constructor?
  return 1; 
}

int Photovore::think(USeconds &wokeAt) {
	//do we need to wait and think and all that?
	// only if we want to make sure we don't get swamped with joystick commands
  int x,y;
  static int cycle;
  static unsigned int light[4];
  int tempt = 0, i;
	// read in the Joystick Control Drop
	stateDrop = ((CritterStateDrop*)lake->readHead(stateInput));
  lake->doneRead(stateInput);

	// translate the joystick controls into robot commands
	// write the robot commands to the drop
	if(stateDrop) {

    light[0] += stateDrop->light[0];
    light[1] += stateDrop->light[1];
    light[2] += stateDrop->light[2];
    light[3] += stateDrop->light[3];
    if(++cycle >= 10) {
      cycle = 0;
      light[0] = light[0] / 10;
      light[1] = light[1] / 10;
      light[2] = light[2] / 10;
      light[3] = light[3] / 10;


    /*if(front_last == 0) {
		  front_last = light[0];
      return 1;
    }*/
    
    if(light[1] > light[0]) {
      tempt -= 10;
    }
    else if(light[2] > light[0]) {
      tempt += 10;
    }
    else if(light[3] > light[0]) {
      tempt += 10;
    }
    if(light[0] > front_last) {
    }
    else {
      dir = -dir;
    }
    if(tempt == 0)
      tempt += dir * 5;

    front_last = light[0];

    x = 0;
    y = 0;

    if(stateDrop->ir_distance[0] > 150) {
      x -= 10;
      y += 0;
    }
    if(stateDrop->ir_distance[1] > 150) {
      x -= 10;
      y -= 5;
    }
    if(stateDrop->ir_distance[2] > 150) {
      x += 0;
      y -= 5;
    }
    if(stateDrop->ir_distance[3] > 150) {
      x += 0;
      y -= 5;
    }
    if(stateDrop->ir_distance[4] > 150) {
      x += 0;
      y += 5;
    }
    if(stateDrop->ir_distance[5] > 150) {
      x += 0;
      y += 5;
    }
    if(stateDrop->ir_distance[6] > 150) {
      x -= 10;
      y += 5;
    }
    if(stateDrop->ir_distance[7] > 150) {
      tempt += 5;
    }
    if(stateDrop->ir_distance[8] > 150) {
      tempt += 5;
    }
    if(stateDrop->ir_distance[9] > 150) {
      tempt -= 5;
    }


    controlDrop.motor_mode = CritterControlDrop::XYTHETA_SPACE;
		controlDrop.x_vel = x + 5;
		controlDrop.y_vel = y ;
		controlDrop.theta_vel = tempt;
		
		//copying James
		(*(CritterControlDrop*)lake->startWriteHead(controlOutput)) = controlDrop;
		lake ->doneWriteHead(controlOutput);
    thinks++;

    light[0] = 0;
    light[1] = 0;
    light[2] = 0;
    light[3] = 0;
    }
	}
  return 1;
}
