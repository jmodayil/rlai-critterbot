#include "SimulatorInterface.h"
#include "CritterControlDrop.h"
#include "CritterStateDrop.h"
#include "NetworkHeader.h"

SimulatorInterface::SimulatorInterface(DataLake *lake, 
                                   ComponentConfig &config, 
                                   string &name) : 
                                   Component(lake, config, name) {
	//	newData = false;
	postWait = 1000000;
	lastPost.setAsNow();
}

SimulatorInterface::~SimulatorInterface() {
	lake->doneWriteHead(stateWrite);
  lake->doneRead(controlRead);
}

int SimulatorInterface::loadConfig(ComponentConfig *config) {
  // see FakeLaser.cpp for ideas about how to read the config
	// file once we have some things that need to be configured
  fprintf (stderr, "Loading SimulatorInterface configuration...\n");
  robotPort = 2323;
  objectivePort = 2324;

  return 1; 
}

int SimulatorInterface::init(USeconds &wokeAt) {
  struct sockaddr_in client;
  int temp, flag;
  string msg;

	stateWrite = lake->readyWriting(CritterStateDrop::name);
  controlRead = lake->readyReading(CritterControlDrop::name);

  objectiveSocket = -1;
  robotSocket = socket(PF_INET, SOCK_STREAM, 0);

  // This code was stolen from ConnectAccept in an attempt to speed things up,
  //  most likely a bad idea - MGB says, I hate sockets in C
  if (robotSocket < 0) {
    msg = "Could not create robot socket.";
    ErrorMessage::sendMessage(lake, getName(), msg + strerror(errno));
    return -1;
  }

  //could not get the flag
  if((flag = fcntl(robotSocket, F_GETFL, 0)) < 0) {
    msg = "Could not get socket flags.";
    ErrorMessage::sendMessage(lake, getName() + "::init()", 
      msg + strerror(errno));
    return -1;
  }

  //could not set the socket to non blocking
  if(fcntl(robotSocket, F_SETFL, flag|O_NONBLOCK) < 0) {
    msg = "Could not set socket flags.";
    ErrorMessage::sendMessage(lake, getName() + "::init()", msg + strerror(errno));
    return -1;
  }

  //turn the nagle algorithm off
  flag = 1;
  //could not set socket options
  if(setsockopt(robotSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int)) < 0){
    msg = "Could not turn naggle off.";
    ErrorMessage::sendMessage(lake, getName() + "::init()", msg + strerror(errno));
    return -1;
  }

  //set the socket options
  memset(&client, 0, sizeof(client));
  client.sin_family = AF_INET;
  // @@@ fixme
  inet_aton("127.0.0.1", &client.sin_addr);
  client.sin_port = 0;

  // bind the socket
  socklen_t dataSize = (socklen_t)sizeof(client);
  if(bind(robotSocket, (struct sockaddr*)(&client), dataSize) < 0) {
    msg = "binding socket to a port failed.";
    ErrorMessage::sendMessage(lake, getName() + "::init()", msg + strerror(errno));
    return -1;
  }

  memset(&client, 0, sizeof(client));
  if(getsockname(robotSocket, (struct sockaddr*)(&client), &dataSize) < 0) {
    msg = "could not get the name of the socket I just created";
    ErrorMessage::sendMessage(lake, getName() + "::init()", msg + strerror(errno));
    return -1;
  }

  // Set up the 'client' structure to contain the simulator server information
  memset(&client, 0, sizeof(client));
  // @@@ fixme - use gethostbyname
  inet_aton("127.0.0.1", &client.sin_addr);
  // @@@ fixme - read port from config file
  client.sin_port = htons(robotPort);

  if(connect(robotSocket, (struct sockaddr*)&client, sizeof(client)) < 0) {
    msg = "connecting to robot server failed.";
    ErrorMessage::sendMessage(lake, getName() + "::init()", 
      msg + strerror(errno));
    return -1;
  }

  //success
  return 1; 
}

int SimulatorInterface::think(USeconds &wokeAt) {
  // 1. Read in CritterControlDrop, pass it along if available
  // 2. Read from simulator, create CritterStateDrop
  return 1;
} 
