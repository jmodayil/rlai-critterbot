/* CritterLogger.cpp
 * Log Critter sessions
 * Mike Sokolsky
 * Created: 12 March, 2009
 */

#include "CritterLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

CritterLogger::CritterLogger(DataLake *lake, ComponentConfig &conf, 
                   string &name) : Component(lake, conf, name) {

  logTagId = lake->readyReading("CritterLogTagDrop");
  controlId = lake->readyReading("CritterControlDrop");
  stateId   = lake->readyReading("CritterStateDrop");
  acts      = 0;
  
}

int CritterLogger::readConfig(ComponentConfig *config) {

  XMLNode *tag;
  XMLError err;

  tag = config->getXMLNode("config", err);
  if( tag == NULL ) 
    return error(err.msg);
  
  if( !tag->getAttrValue( "logDir", log_path, err ) ) 
    return error(err.msg);

  return 1;
}

FILE* CritterLogger::rotate_log( FILE *log, USeconds *now ) {

  char file_name[100];

  strcpy( file_name, log_path.c_str() );
  strcat( file_name, "current_robot.log" );
  
  if(log) {
    fclose(log);
    rename( file_name, file_timestamp.c_str() );
  }
  
  if( !(log = fopen( file_name, "w" )) )  {
    error("Could not open log file: %s\n", file_name);
    return log;
  }

  strcpy( file_name, log_path.c_str() );
  strcat( file_name, (now->formatTime("%y-%m-%d-%H-%M-%S")).c_str() );
  strcat( file_name, LOG_EXTENSION );
  file_timestamp = file_name;
  last_log = *now;
  
  fprintf( log, "#Time Motor0_Command Motor0_Speed Motor0_Current Motor0_Temp Motor1_Command Motor1_Speed Motor1_Current Motor1_Temp Motor2_Command Motor2_Speed Motor2_Current Motor2_Temp AccelX AccelY AccelZ RotationVel IR0 IR1 IR2 IR3 IR4 IR5 IR6 IR7 IR8 IR9 Light0 Light1 Light2 Light3\n" );

  fflush( log );
  
  return log;
}

CritterLogger::~CritterLogger() {
  cleanup();
}

void CritterLogger::cleanup() {

  char file_name[100];
  lake->doneRead(logTagId);
  lake->doneRead(controlId);
  lake->doneRead(stateId);

  if(log) {
    strcpy( file_name, log_path.c_str() );
    strcat( file_name, "current_robot.log" );
    fclose(log);
    rename( file_name, file_timestamp.c_str() );
  }
}

int CritterLogger::init(USeconds &wokeAt) {

  if(readConfig(&config) != 1)
    return error("Error reading configuration data!");

  if( !(log = rotate_log(NULL, &wokeAt)) )
    return -1;
  
  return 1;
}

int CritterLogger::getFID() {
  return 0;
}

void CritterLogger::logData(CritterStateDrop *stateDrop, USeconds *theTime) {

  if(theTime->time.tv_sec >= last_log.time.tv_sec + LOG_INTERVAL * 60 ) {
    fprintf( stderr, "Opening new Log File: %s\n", 
        file_timestamp.c_str() );
    log = rotate_log( log, theTime );
  }
  
  string timestring = theTime->toString();
  fprintf(log, "%s ", (timestring.substr(0,timestring.length() - 1)).c_str());
  fprintf(log, "%d ", stateDrop->motor100.command);
  fprintf(log, "%d ", stateDrop->motor100.velocity);
  fprintf(log, "%d ", stateDrop->motor100.current);
  fprintf(log, "%d ", stateDrop->motor100.temp);
  fprintf(log, "%d ", stateDrop->motor220.command);
  fprintf(log, "%d ", stateDrop->motor220.velocity);
  fprintf(log, "%d ", stateDrop->motor220.current);
  fprintf(log, "%d ", stateDrop->motor220.temp);
  fprintf(log, "%d ", stateDrop->motor340.command);
  fprintf(log, "%d ", stateDrop->motor340.velocity);
  fprintf(log, "%d ", stateDrop->motor340.current);
  fprintf(log, "%d ", stateDrop->motor340.temp);
  fprintf(log, "%d ", stateDrop->accel.x);
  fprintf(log, "%d ", stateDrop->accel.y);
  fprintf(log, "%d ", stateDrop->accel.z);
  fprintf(log, "%d ", stateDrop->rotation);
  for(int i = 0; i < 10; i++)
    fprintf(log, "%d ", stateDrop->ir_distance[i]);
  for(int i = 0; i < 4; i++)
    fprintf(log, "%d ", stateDrop->light[i]);
  fprintf(log, "\n");
}

//logTagDrop = (CritterLogTagDrop*)lake->readHead(logTagId);  
//lake->doneRead(logTagId);

int CritterLogger::act(USeconds &now) {

  stateDrop = (CritterStateDrop*)lake->readHead(stateId);
  lake->doneRead(stateId);

  logTagDrop = (CritterLogTagDrop*)lake->readHead(logTagId);
  lake->doneRead(logTagId);

  if(stateDrop) {
    logData(stateDrop, &now); 
  }
  if(logTagDrop) {
    fprintf(log, "#%s\n", logTagDrop->tagInfo.c_str() );
  }
}
