/* CritterDriver.cpp
 * Seial driver for the main CritterBot hardware
 * James Neufeld (neufeld@cs.ualberta.ca)
 */

#include "CritterDriver.h"
#include "CritterControlDrop.h"
#include "CritterStateDrop.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


CritterDriver::CritterDriver(DataLake *lake, ComponentConfig &conf, 
                   string &name) : Component(lake, conf, name) {

  fid       = -1;
  controlId = lake->readyReading("CritterControlDrop");
  stateId   = lake->readyWriting("CritterStateDrop");
  acts      = 0;
  newData   = false;

  postWait = 10000;

  lastPost.setAsNow();
}

CritterDriver::~CritterDriver() {
  cleanup();
}

void CritterDriver::cleanup() {

  lake->doneRead(controlId);
  lake->doneWriteHead(stateId);

  closeport();
  close(fid);

}

void CritterDriver::initport() {
  
  struct termios options;
  tcflush(fid, TCIOFLUSH);
  tcgetattr(fid, &oldterm);
  tcgetattr(fid, &options);
  options.c_cflag &= ~(CSIZE | CSTOPB | PARENB | CRTSCTS);
  options.c_cflag |= (CS8 | CLOCAL | CREAD);
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_iflag &= (INPCK | IXON | IXOFF);
  options.c_iflag |= IGNPAR | OCRNL;
  options.c_oflag &= ~OPOST; 
  options.c_cc[VTIME] = 0;
  options.c_cc[VMIN] = 1;
  cfsetispeed(&options, BAUDRATE);
  cfsetospeed(&options, BAUDRATE);
  tcflush(fid, TCIOFLUSH);
  tcsetattr(fid, TCSANOW, &options);

}

void CritterDriver::closeport() {

  tcsetattr(fid, TCSANOW, &oldterm);
  tcflush(fid, TCIOFLUSH);

}


int CritterDriver::init(USeconds &wokeAt) {

  printf("Opening serial port.\n");
  if (0 > (fid = open(DEVICE, O_RDWR | O_NOCTTY | O_NDELAY))) {
    printf("Could not open serial port.\n");
    return -1;
  }
  printf("Serial port open with file descriptor %d.\n", fid);
  fcntl(fid, F_SETFL, O_NONBLOCK);
  
  printf("Initializing serial port.\n");
  initport();

  return 0;
}

void CritterDriver::readPacket() {

  unsigned char buf[34];
  
  if(fid > 0) {
    if (34 == read(fid, buf, 34)) {
      int i = 0;
      stateDrop.motor100.velocity = (int) buf[i++];
      stateDrop.motor100.current  = (int) buf[i++];
      stateDrop.motor100.temp     = (int) buf[i++];
      stateDrop.motor220.velocity = (int) buf[i++];
      stateDrop.motor220.current  = (int) buf[i++];
      stateDrop.motor220.temp     = (int) buf[i++];
      stateDrop.motor340.velocity = (int) buf[i++];
      stateDrop.motor340.current  = (int) buf[i++];
      stateDrop.motor340.temp     = (int) buf[i++];
      
      stateDrop.accel.x           = (int) buf[i++];
      stateDrop.accel.y           = (int) buf[i++];
      stateDrop.accel.z           = (int) buf[i++];
      stateDrop.mag.x             = (int) buf[i++];
      stateDrop.mag.y             = (int) buf[i++];
      stateDrop.mag.z             = (int) buf[i++];

      stateDrop.rotation          = (int) buf[i++];
      for(int j=0; j<10; j++) stateDrop.ir_distance[j] = (int) buf[i++];
      for(int j=0; j<4; j++) stateDrop.light[j]        = (int) buf[i++];

      stateDrop.error_flags = 0;
      for(int j=0; j<4; j++) {
        stateDrop.error_flags << 8;
        stateDrop.error_flags += buf[i++];
      }
      newData = true;
    } // else??? - serial must have sent a EOF in the middle of a packet, or timed out
  } else {
    ErrorMessage::sendMessage(lake, getName(), "CritterDriver::readPacket(): Serial port closed unexpectedly.");
  }
}

int CritterDriver::sense(USeconds &wokeAt) {

  unsigned char header[] = {'a','b','c','d'};
  unsigned char buf;

  int i = 0;
  
  if(fid > 0) {
    while(read(fid, &buf, 1) > 0) {
      
      if (buf == header[i]) {
        if (i == 3) readPacket();
        else i++;
      } else {
        i = 0;
      }
      
    }
  } else {
    ErrorMessage::sendMessage(lake, getName(), "sense(): Error reading from serial port.");
    return -1;
  }  
  
}

int CritterDriver::act(USeconds &now) {

  char str[100];
  
  if(now - lastPost >= postWait) {
    lastPost = now + (now - lastPost - postWait);
    acts++;
    
    controlDrop = (CritterControlDrop*)lake->readHead(controlId);  
    
    if (controlDrop) {
      
      if (controlDrop->motor_mode == CritterControlDrop::WHEEL_SPACE) {

        int v1 = controlDrop->m100_vel;
        int v2 = controlDrop->m220_vel;
        int v3 = controlDrop->m340_vel;

        int amax = abs(v1);
        if (abs(v2) > amax) amax = abs(v2);
        if (abs(v3) > amax) amax = abs(v3);
        if (amax > 100) {
          v1 = v1*100/amax;
          v2 = v2*100/amax;
          v3 = v3*100/amax;
        }
        
        sprintf(str,"motor 0 %3d\r",v1);
        if (12 != write(fid, &str, 12)) fprintf(stderr, "write failed! (%s)\n", str); 
        
        sprintf(str,"motor 1 %3d\r",v2);
        if (12 != write(fid, &str, 12)) fprintf(stderr, "write failed! (%s)\n", str); 

        sprintf(str,"motor 2 %3d\r",v3);
        if (12 != write(fid, &str, 12)) fprintf(stderr, "write failed! (%s)\n", str); 
        

      } else { // assert(controlDrop->motor_mode == CritterControlDrop::XYTHETA_SPACE)
        
        int v1,v2,v3; // wheel velocities 

        // [ v1 ]   [ cos(90+100) sin(90+100) 90 ]   [ v_x ]
        // [ v2 ] = [ cos(90+220) sin(90+220) 90 ] * [ v_y ]
        // [ v3 ]   [ cos(90+340) sin(90+340) 90 ]   [ v_theta ]
        v1 = (int)(-0.9848*controlDrop->x_vel + -0.1736*controlDrop->y_vel + 90*controlDrop->theta_vel);
        v2 = (int)(0.6428*controlDrop->x_vel + -0.766*controlDrop->y_vel + 90*controlDrop->theta_vel);
        v3 = (int)(0.342*controlDrop->x_vel + 0.9397*controlDrop->y_vel + 90*controlDrop->theta_vel);


        // scale velocities to max |v| <= 100
        int amax = abs(v1);
        if (abs(v2) > amax) amax = abs(v2);
        if (abs(v3) > amax) amax = abs(v3);
        if (amax > 100) {
          v1 = v1*100/amax;
          v2 = v2*100/amax;
          v3 = v3*100/amax;
        }
        
        sprintf(str,"motor 0 %3d\r", v1);
        if (12 != write(fid, &str, 12)) fprintf(stderr, "write failed! (%s)\n", str); 

        sprintf(str,"motor 0 %3d\r", v2);
        if (12 != write(fid, &str, 12)) fprintf(stderr, "write failed! (%s)\n", str); 

        sprintf(str,"motor 0 %3d\r", v3);
        if (12 != write(fid, &str, 12)) fprintf(stderr, "write failed! (%s)\n", str); 
      
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
