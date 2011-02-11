#ifndef SERVER_H_
#define SERVER_H_

#include <pthread.h>
#include <list>
#include "../seriallink/CritterSerialLink.h"
#include "../drops/CritterControlDrop.h"
#include "SyncActionBuffer.h"

class Server {
public:
	Server(CritterSerialLink& serialLink, SyncActionBuffer& actionBuffer, int port);
	virtual ~Server();
	// Main thread
	bool start();
	// Accept client thread
	void acceptClients();
	// Main thread
	void processStateDrop(const CritterStateDrop& stateDrop);
	// Main thread
	bool sendDropToClient(int clientSocket, char* stateDrop, int size);
	// Action thread
	void listenForAction();
	bool hasClients();
private:
	// Accept client thread
	void addClient(int clientSocket);
	// Main thread
	void removeClient(int clientSocket);
	// Action thread
	void recvActionDrop(int clientSocket);
private:
	CritterSerialLink& serialLink;
	SyncActionBuffer& actionBuffer;
	int acceptingSocket;
	int port;
	pthread_t acceptClientThread;
	// Action stuff
	pthread_t actionClientThread;
	CritterControlDrop controlDrop;
	// Client list stuff
	static pthread_mutex_t clientListMutex;
	std::list<int> clientList;
};

#endif /* SERVER_H_ */
