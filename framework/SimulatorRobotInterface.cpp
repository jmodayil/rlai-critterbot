#include "SimulatorRobotInterface.h"
#include "CritterControlDrop.h"
#include "CritterStateDrop.h"
#include "CritterRewardDrop.h"
#include "CritterDrop.h"
#include "CritterLogTagDrop.h"

#include "XMLNode.h"
#include <string.h>

SimulatorRobotInterfaceProc::SimulatorRobotInterfaceProc(DataLake *lake, 
  ComponentConfig &config, string name) : SocketProtocol(lake, config, name) {
  
  numMapItems = 0;
}

int SimulatorRobotInterfaceProc::readConfig() {
  XMLError err;

  clearWriteData();
  clearReadData();

  return 1;
}

int SimulatorRobotInterfaceProc::init(USeconds &wokeAt) {
  // Add all the drops we care about
  addDropToMap(CritterStateDrop::name, true, true);
  addDropToMap(CritterRewardDrop::name, true, true);
  addDropToMap(CritterControlDrop::name, true, true);
  // addDropToMap(CritterLogTagDrop::name, true, false);

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
        readPtr = readPtr - numBytes; 
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

  char receivedDropName[1024];

  if (nameLength >= sizeof(receivedDropName)) {
    debug("ERROR: Very long drop name, probably garbage.");
    // @todo at this point do something
    return -1;
  }

  strncpy(receivedDropName, data, nameLength);
  // Terminate with a 0
  receivedDropName[nameLength] = 0;

  data += nameLength;
  newDataLength -= nameLength;

  dropMapItem * item = getItemByName(receivedDropName);
  if (item == NULL) {
    // Unsafe! but at this point we should just cry
    debug("ERROR: Unknown drop type %s\n", receivedDropName);
    // Trash the data to avoid repeating
    // @todo this should happen in processRead
    unreadDataPtr += nameLength;
    return -1;
  }

  if (newDataLength < sizeof(int)) return -1;
  newDataLength -= 4;

  // Read the drop length 
  int dropLength = *((int*)data);
  data += sizeof(dropLength);

  // Do we have enough data to parse this drop?
  if (newDataLength < dropLength) return -1;

  if (dropLength < 0) {
    debug("ERROR: Negative drop length, probably garbage.");
    // @todo do something
    return -1;
  }

  // Test if there is enough data in the buffer (length >= dropLength)
  if (newDataLength < dropLength)
    return -1;

  // I hope you defined dropWrite...
  if (!item->dropWrite.defined()) {
    debug("WARNING: No write river defined for drop %s\n", receivedDropName);
  }
  else {
    DataDrop * newDrop = lake->startWriteHead(item->dropWrite);
    newDrop->readArray(data);
    lake->doneWriteHead(item->dropWrite);
  }

  // @todo logtagdrop will presently fail
  newDataLength -= dropLength;

  // Return the number of characters used
  return (int)(readPtr - unreadDataPtr) - newDataLength; 
}

int SimulatorRobotInterfaceProc::act(USeconds & wokeAt)
{
  // Go through the list of possible drops
  for (int i = 0; i < numMapItems; i++) {
    // Get the river we want to read from
    RiverRead river = dropMap[i].dropRead;

    // Ignores drops that don't get read from the system
    if (!river.defined()) continue;

    // Read in a drop of that type
    // @todo add conditional
    DataDrop * drop = lake->readHead(river);

    while (drop != NULL) {
      // Write this drop (and hope it's a CritterDrop)
      writeDrop((CritterDrop*)drop);
      lake->doneRead(river);
      
      // Read in the next drop
      drop = lake->readHead(river);
    }
  
    // Extra doneRead() called when the drop is null so that we have a
    //  matching number of doneRead's and readHead's
    lake->doneRead(river);
  }
  
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

void SimulatorRobotInterfaceProc::addDropToMap(string dropName, 
  bool needsRead, bool needsWrite) {
  // 'create' a new map item
  dropMap[numMapItems].dropName = dropName;
  if (needsWrite)
    dropMap[numMapItems].dropWrite = lake->readyWriting(dropName);
  if (needsRead)
    dropMap[numMapItems].dropRead = lake->readyReading(dropName);

  numMapItems++;
}

dropMapItem * SimulatorRobotInterfaceProc::getItemByName(string dropName) {
  // Try to find it in our list of items
  for (int i = 0; i < numMapItems; i++) {
    if (dropMap[i].dropName == dropName)
      return &dropMap[i];
  }

  return NULL;
}

void SimulatorRobotInterfaceProc::writeDrop(CritterDrop * drop)
{
  // First write the name of the drop to the buffer
  const char * dropName = drop->getName().c_str();
  int len = strlen(dropName);

  char * bufferHead = writePtr;

  // @@@ do bounds-checking - fail if remaining space < sizeof(int) + len + 
  //  drop.getSize()
  // Write the string length as first bit of data
  *((int*)writePtr) = len;
  writePtr += sizeof(len);

  // Then the string itself
  memcpy (writePtr, dropName, len);
  
  writePtr += len;

  // Then the drop size
  int dropSize = drop->getSize();
  *((int*)writePtr) = dropSize; 
  writePtr += sizeof(dropSize);
  
  // Now write the drop data
  drop->writeArray(writePtr);

  writePtr += dropSize; 

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
