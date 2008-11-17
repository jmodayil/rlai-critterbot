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
  clearReadData();

  return 1;
}

int SimulatorRobotInterfaceProc::init(USeconds &wokeAt) {
  stateWrite = lake->readyWriting(CritterStateDrop::name);
  controlRead = lake->readyReading(CritterControlDrop::name);

  return readConfig();
}

char* SimulatorRobotInterfaceProc::writeBuffer(int &size, USeconds &wokeAt) {

  size = writeSize;
  // Purge the buffer for subsequent calls by resetting the size to 0
  clearWriteData();
  
  return writeData;
}

void SimulatorRobotInterfaceProc::clearWriteData()
{
  writeSize = 0;
  writePtr = writeData;
}

void SimulatorRobotInterfaceProc::clearReadData()
{
  readPtr = readData;
  unreadDataPtr = readData;
}

char* SimulatorRobotInterfaceProc::readBuffer(int &size) {
  size = (int)(readData + MAX_ROBOT_INTERFACE_DATA_LENGTH - readPtr);
  // @@@ fprintf (stderr, "Size is %d\n", size);

  return readPtr;
}

int SimulatorRobotInterfaceProc::processRead(char *buf, int size, 
  USeconds &wokeAt) 
{
  // Keep track of partial drop data
  readPtr += size;
  /** @@@ fprintf (stderr, "processRead, now %d left (%d) read in %d\n", 
    readData + MAX_ROBOT_INTERFACE_DATA_LENGTH - readPtr,
    readPtr - readData, size); */

  int numBytes = 0;

  do
  {
    numBytes = processDrop();

    // @@@ fprintf(stderr, "\nNum bytes %d\n", numBytes);

    if (numBytes > 0)
    {
      unreadDataPtr += numBytes;
      if (unreadDataPtr > readPtr)
        debug("FATAL: read more data than available");
      else if (unreadDataPtr == readPtr)
        clearReadData();
      else
      {
        // Shift data over
        // @@@ move out of loop
        int len = (int)(readPtr - unreadDataPtr);

        memcpy (readData, unreadDataPtr, len); 
        // Reset the read pointers accordingly 
        readPtr = readPtr - len; 
        unreadDataPtr = readData;
      }
    }
  }
  while (numBytes > 0) ;

  return 1;
}

int SimulatorRobotInterfaceProc::processDrop()
{
  // See if the data between 'unreadDataPtr' and 'readPtr' is enough for 
  //  a drop
  int newDataLength = (int)(readPtr - unreadDataPtr);
  char * data = unreadDataPtr;

  if (newDataLength < sizeof(int)) return -1;
  newDataLength -= 4;

  // Read the classname length
  int nameLength = *((int*)data);
  data += sizeof(nameLength);

  // Read in the classname
  if (newDataLength < nameLength) return -1;

  bool stateDrop = false;
  int dropLength = -1;

  // @@@ Find a better way to do this - is there a classloader?
  if (strncmp(data, CritterStateDrop::name.c_str(), nameLength) == 0)
  {
    stateDrop = true;
    // This is hardcoded, not as a sign of weakness but rather as a reminder
    //  that the drop length needs to be read from the socket. The reason
    //  is that if we have variable length data, the other (Java) end needs
    //  to tell us the data size
    dropLength = 340;
  }

  if (dropLength < 0)
  {
    // Unsafe! but at this point we should just cry
    data[nameLength] = 0;
    debug("ERROR: Unknown drop type %s\n", data);
    // Trash the data to avoid repeating
    // @@@ this should happen in processRead
    unreadDataPtr += nameLength;
    return -1;
  }

  data += nameLength;
  newDataLength -= nameLength;

  // Test if there is enough data in the buffer (length >= dropLength)
  if (newDataLength < dropLength)
    return -1;

  // Process this drop as a CritterStateDrop
  if (stateDrop)
  {
    CritterStateDrop * newDrop = 
      (CritterStateDrop*)lake->startWriteHead(stateWrite);
    // @@@ fprintf (stderr, "Size of drop is %d\n", newDrop->getSize());

    // Write a new drop to the lake and fill it with the data from the socket
    newDrop->readArray(data);
    lake->doneWriteHead(stateWrite);
  }

  newDataLength -= dropLength;

  // Return the number of characters used
  return (int)(readPtr - unreadDataPtr) - newDataLength; 
}

int SimulatorRobotInterfaceProc::act(USeconds & wokeAt)
{
  // Read in any queued control drops
  CritterControlDrop * ctrlDrop = 
    (CritterControlDrop*)lake->readHead(controlRead);

  while (ctrlDrop != NULL)
  {
    // Write the control drop to the TCP buffer
    writeDrop(ctrlDrop);
    lake->doneRead(controlRead);

    ctrlDrop = (CritterControlDrop*)lake->readHead(controlRead);
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

