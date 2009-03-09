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
#include <time.h>

const unsigned short CritterDriver::crctable[256] = {
	 0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	 0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	 0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	 0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	 0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	 0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	 0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	 0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	 0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	 0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	 0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
	 0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	 0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
	 0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	 0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	 0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
	 0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	 0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	 0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	 0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	 0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	 0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	 0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	 0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	 0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	 0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	 0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	 0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	 0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	 0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	 0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	 0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};	


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

int CritterDriver::readConfig(ComponentConfig *config) {

  XMLNode *tag;
  XMLError err;

  tag = config->getXMLNode("config", err);
  if( tag == NULL ) 
    return error(err.msg);
  
  if( !tag->getAttrValue( "logDir", log_path, err ) ) 
    return error(err.msg);

  if( !tag->getAttrValue( "serialPort", serial_port, err ) )
    return error(err.msg);

  return 1;
}

FILE* CritterDriver::rotate_log( FILE *log, USeconds *now ) {

  char file_name[100];

  if(log)
    fclose(log);

  strcpy( file_name, log_path.c_str() );
  strcat( file_name, (now->formatTime("%y-%m-%d-%H-%M-%S")).c_str() );
  strcat( file_name, LOG_EXTENSION );

  log = fopen( file_name, "a" );
  last_log = *now;
  
  
  fprintf( log, "Time Voltage Motor0_Command Motor0_Speed Motor0_Current Motor0_Temp Motor1_Command Motor1_Speed Motor1_Current Motor1_Temp Motor2_Command Motor2_Speed Motor2_Current Motor2_Temp AccelX AccelY AccelZ RotationVel IR0 IR1 IR2 IR3 IR4 IR5 IR6 IR7 IR8 IR9 Light0 Light1 Light2 Light3\n" );

  fflush( log );
  
  return log;
}

CritterDriver::~CritterDriver() {
  cleanup();
}

void CritterDriver::cleanup() {

  lake->doneRead(controlId);
  lake->doneWriteHead(stateId);

  closeport();
  close(fid);
  fclose(log);

}

void CritterDriver::initport() {
  
  struct termios options;
  tcflush(fid, TCIOFLUSH);
  tcgetattr(fid, &oldterm);
  tcgetattr(fid, &options);
  options.c_cflag &= ~(CSIZE | CSTOPB | CRTSCTS);
  options.c_cflag |= (CS8 | CLOCAL | CREAD | PARENB | PARODD);
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  //options.c_lflag = 0;
  options.c_iflag &= ~(IXON | IXOFF | OCRNL);
  //options.c_iflag = 0;
  options.c_iflag |= (INPCK | PARMRK | IGNBRK);
  //options.c_oflag &= ~OPOST; 
  options.c_oflag = 0;
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

  if(readConfig(&config) != 1)
    return error("Error reading configuration data!");

  printf("Opening serial port.\n");
  if (0 > (fid = open(serial_port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY))) {
    printf("Could not open serial port.\n");
    return -1;
  }
  printf("Serial port open with file descriptor %d.\n", fid);
  fcntl(fid, F_SETFL, O_NONBLOCK);
  
  printf("Initializing serial port.\n");
  initport();
  
  log = rotate_log(NULL, &wokeAt);
  return 1;
}

int CritterDriver::getFID() {
  return fid;
}

void CritterDriver::readPacket( unsigned char buf[], USeconds *theTime) {

      int i = 0;
      unsigned short their_crc;
      

      stateDrop.bus_voltage       = (int) buf[i++];
      stateDrop.motor100.command  = (char) buf[i++];
      stateDrop.motor100.velocity = (char) buf[i++];
      stateDrop.motor100.current  = (int) buf[i++];
      stateDrop.motor100.temp     = (int) buf[i++];
      stateDrop.motor220.command  = (char) buf[i++];
      stateDrop.motor220.velocity = (char) buf[i++];
      stateDrop.motor220.current  = (int) buf[i++];
      stateDrop.motor220.temp     = (int) buf[i++];
      stateDrop.motor340.command  = (char) buf[i++];
      stateDrop.motor340.velocity = (char) buf[i++];
      stateDrop.motor340.current  = (int) buf[i++];
      stateDrop.motor340.temp     = (int) buf[i++];
      
      stateDrop.accel.x           = ((char) buf[i++]) << 4;
      stateDrop.accel.y           = ((char) buf[i++]) << 4;
      stateDrop.accel.z           = ((char) buf[i++]) << 4;
      //stateDrop.mag.x             = ((char) buf[i++]) << 2;
      //stateDrop.mag.y             = ((char) buf[i++]) << 2;
      //stateDrop.mag.z             = ((char) buf[i++]) << 2;

      stateDrop.rotation          = ((char) buf[i++]) << 2;

      for(int j=0; j<10; j++) stateDrop.ir_distance[j] = (int) buf[i++];
      for(int j=0; j<4; j++) stateDrop.light[j]        = ((int) buf[i++]) << 2 ;

      stateDrop.error_flags = buf[i++] << 24;
      stateDrop.error_flags |= buf[i++] << 16;
      stateDrop.error_flags |= buf[i++] << 8;
      stateDrop.error_flags |= buf[i++];
      stateDrop.cycle_time = (int) buf[i++];
      their_crc = ((unsigned short)buf[i++]) << 8;
      their_crc += buf[i++];
//fprintf(stderr, "-- %04x\n", calccrc(buf, STATE_LENGTH - 2));
  //    if(their_crc != calccrc(buf, STATE_LENGTH - 2)) {
//	error("Receive a Corrupt Robot State Packet!");
//	return;
  //    }
      if(theTime->time.tv_sec >= last_log.time.tv_sec + LOG_INTERVAL * 60 ) {
        fprintf( stderr, "Opening new Log File.\n" );
        log = rotate_log( log, theTime );
      }
      
      fprintf(log, "%s ", (theTime->toString()).c_str());
      fprintf(log, "%d ", stateDrop.motor100.command);
      fprintf(log, "%d ", stateDrop.motor100.velocity);
      fprintf(log, "%d ", stateDrop.motor100.current);
      fprintf(log, "%d ", stateDrop.motor100.temp);
      fprintf(log, "%d ", stateDrop.motor220.command);
      fprintf(log, "%d ", stateDrop.motor220.velocity);
      fprintf(log, "%d ", stateDrop.motor220.current);
      fprintf(log, "%d ", stateDrop.motor220.temp);
      fprintf(log, "%d ", stateDrop.motor340.command);
      fprintf(log, "%d ", stateDrop.motor340.velocity);
      fprintf(log, "%d ", stateDrop.motor340.current);
      fprintf(log, "%d ", stateDrop.motor340.temp);
      fprintf(log, "%d ", stateDrop.accel.x);
      fprintf(log, "%d ", stateDrop.accel.y);
      fprintf(log, "%d ", stateDrop.accel.z);
      fprintf(log, "%d ", stateDrop.rotation);
      for(int i = 0; i < 10; i++)
        fprintf(log, "%d ", stateDrop.ir_distance[i]);
      for(int i = 0; i < 4; i++)
        fprintf(log, "%d ", stateDrop.light[i]);
      fprintf(log, "\n");
      (*(CritterStateDrop*)lake->startWriteHead(stateId)) = stateDrop;  
      lake->doneWriteHead(stateId);
}

unsigned short CritterDriver::calccrc(unsigned char* data, int size) {

  unsigned short crc = 0;

  if(size < 1)
    return 0;

  while(size--) {
    crc = (crc<<8) ^ crctable[(crc >> 8) ^ *data++];  
  }

  return crc;

}

int CritterDriver::sense(USeconds &wokeAt) {

  static enum {HEADER, DATA, FLAG} state;
  unsigned char header[] = {SER_HEADER1, SER_HEADER2, SER_HEADER3, SER_HEADER4};
  static unsigned char buf;

  static int i;
  
  if(fid <= 0)
    return error("sense(): Error reading from serial port.  FID: %d", fid);
  
  while(read(fid, &buf, 1) == 1) {
    switch(state) {
      case HEADER:
        if(buf == header[i]) {
          if(i >= 3) {
            i = 0;
            state = DATA;
          }
	  else
	    i++;
          break;
        }
	      fprintf(stderr, "Misaligned Packet!!!\n");
        break;
      case DATA:
        if(buf == 0xFF) {
	  state = FLAG;
	  break;
	}
        state_buf[i++] = buf;
	if(i == STATE_LENGTH) {
    readPacket(state_buf, &wokeAt);
	  i = 0;
	  state = HEADER;
	}
	break;
      case FLAG:
        if(buf == 0xFF) {
	  state_buf[i++] = buf;
		if(i == STATE_LENGTH) {
		  readPacket(state_buf, &wokeAt);
		  i = 0;
		  state = HEADER;
		  break;
		}
	  state = DATA;
	  break;
	}
	else if(buf == 0x00) {
	  if(read(fid, &buf, 1) == 1)
	    fprintf(stderr, "Parity error on buf: %u\n", buf);
	  else 
	    fprintf(stderr, "Error reading from serial port?!?!\n");
	  state = HEADER;
	  i = 0;
	  break;
	}
	else {
	  fprintf(stderr, "Error, not sure how this could happen, buf: %u", buf);
	  i = 0;
	  state = HEADER;
	  break;
	}
      default:
        i = 0;
	state = HEADER;
	break;
    }
  }

  return 1; 
}

int CritterDriver::act(USeconds &now) {

  unsigned char sdata[] = {SER_HEADER1, SER_HEADER2, SER_HEADER3, SER_HEADER4,
    0, 0, 0, 0, 0};
 
  unsigned char leddata[NUM_LEDS * 3];

  if(now - lastPost >= postWait) {
    lastPost = now + (now - lastPost - postWait);
    acts++;
    
    controlDrop = (CritterControlDrop*)lake->readHead(controlId);  
    lake->doneRead(controlId);
    
    if (controlDrop) {
      //fprintf(stderr, "X: %d Y: %d Theta: %d\n", (char)controlDrop->x_vel,
      //  (char)controlDrop->y_vel, (char)controlDrop->theta_vel);
      sdata[4] = (char)controlDrop->motor_mode;
      sdata[5] = (char)-controlDrop->x_vel;
      sdata[6] = (char)-controlDrop->y_vel;
      sdata[7] = (char)controlDrop->theta_vel;
      sdata[8] = (char)controlDrop->led_mode;
      //fprintf(stderr,"led_mode: %d\n", (char)controlDrop->led_mode );

      if(controlDrop->led_mode == CritterControlDrop::CUSTOM) {
        for(int i = 0; i < NUM_LEDS; i++) {
          leddata[3*i] = controlDrop->led_val[i].r;
          leddata[3*i + 1] = controlDrop->led_val[i].g;
          leddata[3*i + 2] = controlDrop->led_val[i].b;
        }  
      }
      fprintf(stderr,"%3d %3d %3d\n",leddata[45], leddata[46], leddata[47]);

      if(9 != write(fid, &sdata, 9)) 
          fprintf(stderr, "Error writing data out to serial port!\n"); 
      
      if(48 != write(fid, &leddata, 48))
          fprintf(stderr, "Error writing data out to serial port!\n");     
    }
  }
  return 1;
}
