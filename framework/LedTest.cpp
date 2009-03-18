#include "LedTest.h"
#include <math.h>

LedTest::LedTest(DataLake *lake, 
                                   ComponentConfig &config, 
                                   string &name) : 
                                   Component(lake, config, name) {
	fid = -1;
	stateInput = lake->readyReading("CritterStateDrop");
	imageInput = lake->readyReading("ColorImageDrop");
	controlOutput = lake->readyWriting("CritterControlDrop");
	thinks = 0;
	newData = false;
  front_last = 0;	
  dir = 1;
}

LedTest::~LedTest() {
	//might want to make an array of these and just iterate?
	lake -> stopReading(stateInput);
	lake -> stopReading(imageInput);
	lake -> stopWriting(controlOutput);
}

int LedTest::loadConfig(ComponentConfig *config) {
  // see FakeLaser.cpp for ideas about how to read the config
	// file once we have some things that need to be configured
  

  return 1; 
}

int LedTest::init(USeconds &wokeAt) {
	// why would I do things in init rather than the constructor?
  return 1; 
}

int LedTest::think(USeconds &wokeAt) {
	//do we need to wait and think and all that?
	// only if we want to make sure we don't get swamped with joystick commands
  int x,y;
  unsigned char *pix;
	// read in the Joystick Control Drop
	stateDrop = ((CritterStateDrop*)lake->readHead(stateInput));
  lake->doneRead(stateInput);
  image = ((ColorImageDrop*)lake->readHead(imageInput));
  lake->doneRead(imageInput);
	// translate the joystick controls into robot commands
	// write the robot commands to the drop
	//if(image) {

    controlDrop.motor_mode = CritterControlDrop::XYTHETA_SPACE;
		controlDrop.x_vel = 0;
		controlDrop.y_vel = 0;
		controlDrop.theta_vel = 0;
    controlDrop.led_mode = CritterControlDrop::CUSTOM;
    for( x = 0; x < 16; x++ ) {
      controlDrop.led_val[x].r = 0;
      controlDrop.led_val[x].g = 0;
      controlDrop.led_val[x].b = 0;
    } 
      controlDrop.led_val[0].r = 255;
      controlDrop.led_val[0].g = 0;
      controlDrop.led_val[0].b = 0;
      controlDrop.led_val[4].r = 0;
      controlDrop.led_val[4].g = 0;
      controlDrop.led_val[4].b = 255;
      controlDrop.led_val[8].r = 0;
      controlDrop.led_val[8].g = 0;
      controlDrop.led_val[8].b = 255;
      controlDrop.led_val[12].r = 0;
      controlDrop.led_val[12].g = 0;
      controlDrop.led_val[12].b = 255;

		//copying James
		(*(CritterControlDrop*)lake->startWriteHead(controlOutput)) = controlDrop;
		lake ->doneWriteHead(controlOutput);
    thinks++;
	//}
  return 1;
}
