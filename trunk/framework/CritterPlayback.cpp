/* CritterPlayback.cpp
 * Playback Critter sessions
 * Mike Sokolsky
 * Created: 12 March, 2009
 */

#include "CritterPlayback.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

CritterPlayback::CritterPlayback(DataLake *lake, ComponentConfig &conf, 
                   string &name) : Component(lake, conf, name) {

  logTagId = lake->readyWriting("CritterLogTagDrop");
  stateId   = lake->readyWriting("CritterStateDrop");
  wakeTime.setAsNow();
  paused = false;
}

int CritterPlayback::readConfig(ComponentConfig *config) {

  XMLNode *tag;
  XMLError err;

  tag = config->getXMLNode("config", err);
  if( tag == NULL ) 
    return error(err.msg);
  
  if( !tag->getAttrValue( "logFile", log_file, err ) ) 
    return error(err.msg);

  // We don't care if this isn't specified in the config,
  // as we default to 1 anyway.
  if(!tag->getAttrValue( "timeScale", scale, err ))
    scale = 1; 
  
  return 1;
}

/*
 * Open a new logfile
 */
FILE* CritterPlayback::openFile(string fileName) {

  closeFile();
  if( !(log = fopen( fileName.c_str(), "r" )) ) {
    error("Could not open log file: %s\n", fileName.c_str());
  }
  return log;
}  

CritterPlayback::~CritterPlayback() {
  cleanup();
}

/*
 * Close current log file
 */
void CritterPlayback::closeFile() {
  if(log)
    fclose(log);
}

void CritterPlayback::cleanup() {

  lake->doneWriteHead(logTagId);
  lake->doneWriteHead(stateId);

  closeFile();
}

int CritterPlayback::init(USeconds &wokeAt) {

  if(readConfig(&config) != 1)
    return error("Error reading configuration data!");

  if(openFile(log_file) <= 0)
    return error("Could not open log file!");

  if(readLog() == EOF)
    return error("Error reading from log file");

  startTime = stateDrop.time;
  wallStartTime = wokeAt;
  return 1;
}

int CritterPlayback::getFID() {
  return 0;
}

/*
 * Read a line from the current logfile, ignoring comments and 
 * malformed lines.
 *
 * Returns 0 on success or EOF
 */
int CritterPlayback::readLog(void) {
  
  const int NUM_FIELDS = 33;
  char comment;
  int buf;
  
  if(!log)
    return EOF;

  do {
    if(fscanf(log,"%c",&comment) != 1)
      return EOF;
    if((char)comment == '#') {
      do{
        buf = fgetc(log);
        if(buf == EOF)
          return EOF;
      }
      while((char)buf != '\n');
    }
    else
      fseek(log,-1,SEEK_CUR);
     
    buf = fscanf(log, 
      "%d.%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
      &(stateDrop.time.time.tv_sec),
      &(stateDrop.time.time.tv_usec),
      &(stateDrop.bus_voltage),
      &(stateDrop.motor100.command),
      &(stateDrop.motor100.velocity),
      &(stateDrop.motor100.current),
      &(stateDrop.motor100.temp),
      &(stateDrop.motor220.command),
      &(stateDrop.motor220.velocity),
      &(stateDrop.motor220.current),
      &(stateDrop.motor220.temp),
      &(stateDrop.motor340.command),
      &(stateDrop.motor340.velocity),
      &(stateDrop.motor340.current),
      &(stateDrop.motor340.temp),
      &(stateDrop.accel.x),
      &(stateDrop.accel.y),
      &(stateDrop.accel.z),
      &(stateDrop.rotation),
      &(stateDrop.ir_distance[0]),
      &(stateDrop.ir_distance[1]),
      &(stateDrop.ir_distance[2]),
      &(stateDrop.ir_distance[3]),
      &(stateDrop.ir_distance[4]),
      &(stateDrop.ir_distance[5]),
      &(stateDrop.ir_distance[6]),
      &(stateDrop.ir_distance[7]),
      &(stateDrop.ir_distance[8]),
      &(stateDrop.ir_distance[9]),
      &(stateDrop.light[0]),
      &(stateDrop.light[1]),
      &(stateDrop.light[2]),
      &(stateDrop.light[3]));
  }
  while(buf != NUM_FIELDS);
  
  // Correct for the fact that log file time precision is in
  // ms and stateDrop.time is in us.
  stateDrop.time.time.tv_usec *= 1000; 
  
  return 0; 
}

int CritterPlayback::think(USeconds &now) {

  // Wait 10 ms and try again if paused.
  // @TODO implement the pause feature, right now no way to get out of a 
  // paused state
  if(paused) {
    wakeTime = now;
    wakeTime.time.tv_usec += 10000;
    return 1;
  }
  
  // @TODO implement mechanism to open another log file.
  if( readLog() == EOF) {
    if(log)
      fclose(log);
    paused = true;
    return 1;
  }

  (*(CritterStateDrop*)lake->startWriteHead(stateId)) = stateDrop;
  lake->doneWriteHead(stateId);

  wakeTime = now + ((stateDrop.time - startTime) - 
      ((now - wallStartTime) *= scale));
  thinks++;
  return 1;
}

/*
 * Change the rate at which the logfile is played back.
 * Increasing this too much may cause intermittant CritterStateDrop
 * loss.
 * @TODO allow log files to be played backwards.
 */
void CritterPlayback::setSpeed(int speed, USeconds now) {
  if(speed < 1)
    speed = 1;
  
  // Limit this for now
  if(speed > 8)
    speed = 8;

  scale = speed;
  wallStartTime = now;
  startTime = stateDrop.time;
}

/*
 * Start playing if paused
 */
void CritterPlayback::play(USeconds now) {
  if(paused) {
    wallStartTime = now;
    startTime = stateDrop.time;
    paused = false;
  }
}

/*
 * Pause playback of logfile
 */
void CritterPlayback::pause(void) {
  paused = true;
}

BitMask CritterPlayback::type() {
  BitMask mask;
  mask.maskBit(Component::ComponentThink);
  mask.maskBit(Component::ComponentWake);
  return mask;
}
