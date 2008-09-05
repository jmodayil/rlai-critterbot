/* CritterDriver_debug.cpp
 * Seial driver for the main CritterBot hardware
 * James Neufeld (neufeld@cs.ualberta.ca)
 */

#include "CritterDriver_debug.h"
#include "CritterControlDrop.h"
#include "CritterStateDrop.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


CritterDriver_debug::CritterDriver_debug(DataLake *lake, ComponentConfig &conf, 
                   string &name) : Component(lake, conf, name) {

  fid       = -1;
  controlId = lake->readyReading("CritterControlDrop");
  stateId   = lake->readyWriting("CritterStateDrop");
  acts      = 0;
  newData   = false;

  postWait = 10000;

  lastPost.setAsNow();
}

CritterDriver_debug::~CritterDriver_debug() {
  cleanup();
}

void CritterDriver_debug::cleanup() {

  lake->doneRead(controlId);
  lake->doneWriteHead(stateId);

}

void CritterDriver_debug::initport() {
  
}

void CritterDriver_debug::closeport() {

}


int CritterDriver_debug::init(USeconds &wokeAt) {

  printf("Opening FAKE serial port. weeee\n");

  printf("Serial port open with file descriptor %d.\n", fid);
  
  printf("Initializing FAKE serial port.\n");

  return 1;
}

void CritterDriver_debug::readPacket() {  

  stateDrop.motor100.velocity = 10;
  stateDrop.motor100.current  = 10;
  stateDrop.motor100.temp     = 10;
  stateDrop.motor220.velocity = 20;
  stateDrop.motor220.current  = 20;
  stateDrop.motor220.temp     = 20;
  stateDrop.motor340.velocity = 30;
  stateDrop.motor340.current  = 30;
  stateDrop.motor340.temp     = 30;
  
  stateDrop.accel.x           = 10;
  stateDrop.accel.y           = 20;
  stateDrop.accel.z           = 30;
  stateDrop.mag.x             = 11;
  stateDrop.mag.y             = 21;
  stateDrop.mag.z             = 31;
  
  stateDrop.rotation          = 12;
  for(int j=0; j<10; j++) stateDrop.ir_distance[j] = j*10;
  for(int j=0; j<4; j++) stateDrop.light[j]        = j*11;
  
  stateDrop.error_flags = 0;
  for(int j=0; j<4; j++) {
    stateDrop.error_flags << 8;
    stateDrop.error_flags += j;
  }
  newData = true;

}

int CritterDriver_debug::think(USeconds &wokeAt) {
  sense(wokeAt);
  return 1;
}

int CritterDriver_debug::sense(USeconds &wokeAt) {

  readPacket();
  return 1;
  
}

int CritterDriver_debug::act(USeconds &now) {

  char str[100];
  
  if(now - lastPost >= postWait) {
    lastPost = now + (now - lastPost - postWait);
    acts++;
    
    controlDrop = (CritterControlDrop*)lake->readHead(controlId);  
    
    if (controlDrop) {
      
      if (controlDrop->motor_mode == CritterControlDrop::WHEEL_SPACE) {

        sprintf(str,"motor 0 %3d\r",controlDrop->m100_vel);
        printf("CritterDriver_debug: FAKE wrote command: %s\n", str);
        
        sprintf(str,"motor 1 %3d\r",controlDrop->m220_vel);
        printf("CritterDriver_debug: FAKE wrote command: %s\n", str);

        sprintf(str,"motor 2 %3d\r",controlDrop->m340_vel);
        printf("CritterDriver_debug: FAKE wrote command: %s\n", str);
        

      } else { // assert(controlDrop->motor_mode == CritterControlDrop::XYTHETA_SPACE)
        
        int v1,v2,v3; // wheel velocities 

        // [ v1 ]   [ cos(90+100) sin(90+100) 90 ]   [ v_x ]
        // [ v2 ] = [ cos(90+220) sin(90+220) 90 ] * [ v_y ]
        // [ v3 ]   [ cos(90+340) sin(90+340) 90 ]   [ v_theta ]
        v1 = (int)(-0.9848*controlDrop->x_vel + -0.1736*controlDrop->y_vel + 90*controlDrop->theta_vel);
        v2 = (int)(0.6428*controlDrop->x_vel + -0.766*controlDrop->y_vel + 90*controlDrop->theta_vel);
        v3 = (int)(0.342*controlDrop->x_vel + 0.9397*controlDrop->y_vel + 90*controlDrop->theta_vel);

        sprintf(str,"motor 0 %3d\r", v1);
        printf("CritterDriver_debug: FAKE wrote command: %s\n", str);

        sprintf(str,"motor 1 %3d\r", v2);
        printf("CritterDriver_debug: FAKE wrote command: %s\n", str);

        sprintf(str,"motor 2 %3d\r", v3);
        printf("CritterDriver_debug: FAKE wrote command: %s\n", str);
      
      }
    }
    lake->doneRead(controlId);

    if (newData) {
      (*(CritterStateDrop*)lake->startWriteHead(stateId)) = stateDrop;  
      lake->doneWriteHead(stateId);
      newData = false;
    }
  }
  return 1;
}
