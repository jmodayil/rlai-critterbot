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


CritterDriver::CritterDriver(DataLake *lake, ComponentConfig &conf, 
                   string &name) : Component(lake, conf, name) {

  fid       = -1;
  controlId = lake->readyReading("CritterControlDrop");
  stateId   = lake->readyWriting("CritterStateDrop");
  thinks    = 0;

  postWait = 100000;

  lastPost.setAsNow();
}

CritterDriver::~CritterDriver() {
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
  initport(fid);


  return 0;
}


int CritterDriver::sense(USeconds &wokeAt) {
  
  char buf;

  if(fid > 0) {
    while(read(fid, &buf, 1) > 0) {
      
      
      
{
      ErrorMessage::sendMessage(lake, getName(), "sense(): Error reading from serial port.");
      return -1;
    }
  }
  else
    return -1;
  
  

}

int CritterDriver::think(USeconds &now) {

  char str[100];
  
  if(now - lastPost >= postWait) {
    lastPost = now + (now - lastPost - postWait);
    thinks++;
    
    controlDrop = (CritterControlDrop*)lake->readHead(controlId);  
    
    if (controlDrop) {
      // do whatever with struct

      if (controlDrop->motor_mode == CritterControlDrop::WHEEL_SPACE) {

        sprintf(str,"motor 0 %3d\r",controlDrop->m100_vel);
        if (12 != write(fid, &str, 12)) fprintf(stderr, "write failed! (%s)\n", str); 
        
        sprintf(str,"motor 1 %3d\r",controlDrop->m220_vel);
        if (12 != write(fid, &str, 12)) fprintf(stderr, "write failed! (%s)\n", str); 

        sprintf(str,"motor 2 %3d\r",controlDrop->m340_vel);
        if (12 != write(fid, &str, 12)) fprintf(stderr, "write failed! (%s)\n", str); 
        

      } else { // assert(controlDrop->motor_mode == CritterControlDrop::XYTHETA_SPACE)
        
        int v1,v2,v3; // wheel velocities 

        // [ v1 ]   [ cos(90+100) sin(90+100) 90 ]   [ v_x ]
        // [ v2 ] = [ cos(90+220) sin(90+220) 90 ] * [ v_y ]
        // [ v3 ]   [ cos(90+340) sin(90+340) 90 ]   [ v_theta ]
        v1 = (int)(-0.9848*controlDrop->x_vel + -0.1736*controlDrop->y_vel + 90*controlDrop->theta_vel);
        v2 = (int)(0.6428*controlDrop->x_vel + -0.766*controlDrop->y_vel + 90*controlDrop->theta_vel);
        v3 = (int)(0.342*controlDrop->x_vel + 0.9397*controlDrop->y_vel + 90*controlDrop->theta_vel);

        sprintf(str,"motor 0 %3d\r", v1);
        if (12 != write(fid, &str, 12)) fprintf(stderr, "write failed! (%s)\n", str); 

        sprintf(str,"motor 0 %3d\r", v2);
        if (12 != write(fid, &str, 12)) fprintf(stderr, "write failed! (%s)\n", str); 

        sprintf(str,"motor 0 %3d\r", v3);
        if (12 != write(fid, &str, 12)) fprintf(stderr, "write failed! (%s)\n", str); 
      
      }
      
      
    }

    lake->doneRead(controlId);
  }

  return 1;
}
