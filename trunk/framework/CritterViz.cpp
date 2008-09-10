#include "CritterViz.h"

CritterViz::CritterViz(DataLake *lake, 
                                   ComponentConfig &config, 
                                   string &name) : 
                                   Component(lake, config, name) {

	//	newData = false;
	postWait = 1000000;
	lastPost.setAsNow();
}

CritterViz::~CritterViz() {
	//might want to make an array of these and just iterate?
	lake -> doneRead(critterOutput);
}

int CritterViz::loadConfig(ComponentConfig *config) {
  // see FakeLaser.cpp for ideas about how to read the config
	// file once we have some things that need to be configured
  return 1; 
}

int CritterViz::init(USeconds &wokeAt) {
	critterOutput = lake->readyReading("CritterStateDrop");
  return 1; 
}

int CritterViz::think(USeconds &wokeAt) {
	CritterStateDrop *drop = ((CritterStateDrop*)lake->readHead(critterOutput));
	char motorHeader[] = "motor     | velocity | current  | temp     \r";
	char coordHeader[] = "          | x        | y        | z        \r";
	int i;

	if(wokeAt - lastPost >= postWait) {
		lastPost = wokeAt;
		thinks++;

	if(drop) {
		printf("Rotation: %d\n\n",drop->rotation);
		//printf("%s", drop->toString().c_str());
		printf("%s\n", motorHeader);
		printf("100       | %-8d | %-8d | %-8d |\n",drop->motor100.velocity,drop->motor100.current,drop->motor100.temp);
		printf("220       | %-8d | %-8d | %-8d |\n",drop->motor220.velocity,drop->motor220.current,drop->motor220.temp);
		printf("340       | %-8d | %-8d | %-8d |\n\n",drop->motor340.velocity,drop->motor340.current,drop->motor340.temp);
		printf("%s\n", coordHeader);
		printf("accel     | %-8d | %-8d | %-8d |\n",drop->accel.x,drop->accel.y,drop->accel.z);
		printf("mag       | %-8d | %-8d | %-8d |\n\n",drop->mag.x,drop->mag.y,drop->mag.z);
		//printf("ir distance: %d\n\n",drop->ir_distance);
		printf("ERROR FLAG: %d\n\n",drop->error_flags);

		printf("IR distance:");
		for (i=0; i<10; ++i)
			printf(" %d",drop->ir_distance[i]);
		printf("\n\nIR light: ");
		for (i=0; i<8; ++i)
			printf(" %d",drop->ir_light[i]);
		printf("\n\nLight: ");
		for (i=0; i<4; ++i)
			printf(" %d",drop->light[i]);
		printf("\n\n");
	}			
	//ir_distance[10];
	//ir_light[8];
	//light[4];

	//}
 	//these should be cast as a un/signed char
	//IR distances sensors
	//Ambient light sensors 
}
	//Error flags (int value)
  return 1;
} 
