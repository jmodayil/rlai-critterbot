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
  acts      = 0;
}

int CritterPlayback::readConfig(ComponentConfig *config) {

  XMLNode *tag;
  XMLError err;

  tag = config->getXMLNode("config", err);
  if( tag == NULL ) 
    return error(err.msg);
  
  if( !tag->getAttrValue( "logFile", log_file, err ) ) 
    return error(err.msg);

  return 1;
}

FILE* CritterPlayback::openFile(string fileName) {

  if( !(log = fopen( fileName.c_str(), "r" )) ) {
    error("Could not open log file: %s\n", fileName.c_str());
  }
  return log;
}  

CritterPlayback::~CritterPlayback() {
  cleanup();
}

void CritterPlayback::cleanup() {

  lake->doneWriteHead(logTagId);
  lake->doneWriteHead(stateId);

  if(log) {
    fclose(log);
  }
}

int CritterPlayback::init(USeconds &wokeAt) {

  if(readConfig(&config) != 1)
    return error("Error reading configuration data!");

  if(openFile(log_file) <= 0)
    return error("Could not open log file!");

  if(readLog() <= 0)
    return error("Error reading from log file");

  startTime = stateDrop.time;
  wallStartTime = wokeAt;
  return 1;
}

int CritterPlayback::getFID() {
  return 0;
}

int CritterPlayback::readLog(void) {
  
  char time[20];
  char comment;
  fscanf(log,"%c",&comment);
  if(comment == '#')
    while(fgetc(log) != '\n');
  else
    fseek(log,-1,SEEK_CUR);
 
  int retVal = fscanf(log, 
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

  // Correct for the fact that log file time precision is in
  // ms and stateDrop.time is in us.
  stateDrop.time.time.tv_usec *= 1000; 
  return retVal; 
}

int CritterPlayback::act(USeconds &now) {

  if(now - wallStartTime > stateDrop.time - startTime)  {
    fprintf(stderr, "Wall elapsed: %d.%06d, Log elapsed: %d.%06d\n",
        (now - wallStartTime).time.tv_sec,
        (now - wallStartTime).time.tv_usec,
        (stateDrop.time - startTime).time.tv_sec,
        (stateDrop.time - startTime).time.tv_usec);
    if( readLog() <= 0)
      return error("Error reading from log file");   
    (*(CritterStateDrop*)lake->startWriteHead(stateId)) = stateDrop;
    lake->doneWriteHead(stateId);
  
  //wakeTime = now;//+ ((stateDrop.time - startTime) - (now - wallStartTime));
  //wakeTime.time.tv_usec += 100000;
  //fprintf(stderr, "Now: %s, wakeTime: %s\n", now.toString().c_str(),
  //    wakeTime.toString().c_str());
  }
}
