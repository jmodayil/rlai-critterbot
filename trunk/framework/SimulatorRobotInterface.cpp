#include "SimulatorRobotInterface.h"
#include "CritterControlDrop.h"
#include "CritterStateDrop.h"

#include "XMLNode.h"
#include <string.h>

SimulatorRobotInterfaceProc::SimulatorRobotInterfaceProc(DataLake *lake, 
  ComponentConfig &config, string name) : SocketProtocol(lake, config, name) {
}

int SimulatorRobotInterfaceProc::readConfig() {
  XMLError err;

  clearWriteData();

  return 1;
}

int SimulatorRobotInterfaceProc::init(USeconds &wokeAt) {
  stateWrite = lake->readyWriting(CritterStateDrop::name);
  controlRead = lake->readyReading(CritterControlDrop::name);

  return readConfig();
}

char* SimulatorRobotInterfaceProc::writeBuffer(int &size, USeconds &wokeAt) {

  size = writeSize;
  fprintf (stderr, "Writebuffer %d\n", size);
  // Purge the buffer for subsequent calls by resetting the size to 0
  clearWriteData();
  
  return writeData;
}

void SimulatorRobotInterfaceProc::clearWriteData()
{
  writeSize = 0;
  writePtr = writeData;
}

char* SimulatorRobotInterfaceProc::readBuffer(int &size) {
  size = MAX_ROBOT_INTERFACE_DATA_LENGTH;
  return readData;
}

int SimulatorRobotInterfaceProc::processRead(char *buf, int size, 
  USeconds &wokeAt) {
  // @@@ make a drop out of it! or let sense() do it?
  return debug("read in %d bytes", size);
}

int SimulatorRobotInterfaceProc::act(USeconds & wokeAt)
{
  // Read in any queued control drops
  CritterControlDrop * ctrlDrop = 
    (CritterControlDrop*)lake->readHead(controlRead);

  while (ctrlDrop != NULL)
  {
    fprintf(stderr, "Kazaam! Got a control drop with theta=%d\n",
      ctrlDrop->theta_vel);
    // Write the control drop to the TCP buffer
    writeDrop(ctrlDrop);
    lake->doneRead(controlRead);

    ctrlDrop = (CritterControlDrop*)lake->readHead(controlRead);
    fprintf(stderr, "Ta-da, sent away\n");
  }

  // Extra doneRead() called when the drop is null so that we have a
  //  matching number of doneRead's and readHead's
  lake->doneRead(controlRead);

  return 1;
}


int SimulatorRobotInterfaceProc::reloadConfig(ComponentConfig *conf) {
  config = *conf;
  return readConfig(); 
}

Socket* SimulatorRobotInterfaceProc::createClient(DataLake *lake, 
  ComponentConfig &config, string name) {

  return new SimulatorRobotInterface(lake, config, name);
}

void SimulatorRobotInterfaceProc::writeDrop(DataDrop * drop)
{
  // @@@ Oops - no easy to know what kind of drop this is? No problem, 
  //  let's assume the programmer gave us a CritterControlDrop
  // Of course, this is a terrible programming practice
  CritterControlDrop * ctrlDrop = (CritterControlDrop*) drop;
  // First write the name of the drop to the buffer
  const char * dropName = ctrlDrop->name.c_str();
  int len = strlen(dropName);

  char * bufferHead = writePtr;

  // @@@ do bounds-checking - fail if left space < sizeof(int) + len + 
  //  drop.getSize()
  // Write the string length as first bit of data
  *((int*)writePtr) = len;
  writePtr += sizeof(len);

  // Then the string itself
  memcpy (writePtr, dropName, len);
  
  writePtr += len;
  // Now write the drop data
  ctrlDrop->writeArray(writePtr);

  writePtr += ctrlDrop->getSize();

  // Keep track of how many bytes we just added to the buffer
  writeSize += (int)(writePtr - bufferHead);
}



SimulatorRobotInterface::SimulatorRobotInterface(DataLake *lake, 
  ComponentConfig &config, string name) : Socket(lake, config, name) 
{
  wait = 0;

  // Store the protocol we are using, as Socket doesn't allow us to 
  //  retrieve it
  myProtocol = new SimulatorRobotInterfaceProc(lake, config, name);
  setProtocol(myProtocol);
}

int SimulatorRobotInterface::act(USeconds & wokeAt)
{
  // Read any queued drops and generally process stuff
  // Is it really evil to call a Component's act myself?
  if (wakeTime <= wokeAt)
  {
    myProtocol->act(wokeAt);
    Socket::act(wokeAt);
  }

  wakeTime += wait - wokeAt - wakeTime;
}

