#include "Server.h"

#include <iostream>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

pthread_mutex_t Server::clientListMutex = PTHREAD_MUTEX_INITIALIZER;

void *staticAcceptClients(void *ptr) {
	((Server*) ptr)->acceptClients();
	return NULL;
}

void *staticListenForAction(void *ptr) {
	((Server*) ptr)->listenForAction();
	return NULL;
}

Server::Server(CritterSerialLink& serialLink, SyncActionBuffer& actionBuffer, int port) :
	serialLink(serialLink), actionBuffer(actionBuffer) {
	this->port = port;
}

Server::~Server() {
}

bool Server::start() {
	acceptingSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (acceptingSocket < 0) {
		perror("error opening socket");
		return false;
	}
	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	if (bind(acceptingSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr))
			< 0) {
		perror("error on binding the accepting socket");
		return false;
	}
	int ret = pthread_create(&acceptClientThread, NULL, staticAcceptClients,
			(void *) this);
	if (ret != 0) {
		perror("error starting accept thread");
		return false;
	}
	ret = pthread_create(&actionClientThread, NULL, staticListenForAction,
			(void *) this);
	if (ret != 0) {
		perror("error starting action thread");
		return false;
	}
	return true;
}

void Server::acceptClients() {
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	while (true) {
		::listen(acceptingSocket, 1);
		clilen = sizeof(cli_addr);
		int clientSocket = accept(acceptingSocket, (struct sockaddr *) &cli_addr,
				&clilen);
		if (clientSocket < 0)
			perror("error starting action thread");
		std::cout << "Accepting new client" << std::endl;
		int opts = fcntl(clientSocket, F_GETFL);
		if (opts < 0) {
			perror("fcntl(F_GETFL)");
			continue;
		}
		opts = (opts | O_NONBLOCK);
		if (fcntl(clientSocket, F_SETFL, opts) < 0) {
			perror("fcntl(F_SETFL)");
			continue;
		}
		addClient(clientSocket);
	}
}

void Server::listenForAction() {
	struct timeval timeout;
	fd_set sockets;
	while (true) {
		pthread_mutex_lock(&clientListMutex);
		std::list<int> clientListCopied = clientList;
		pthread_mutex_unlock(&clientListMutex);
		FD_ZERO(&sockets);
		int maxFD = 0;
		for (std::list<int>::iterator it = clientListCopied.begin(); it
				!= clientListCopied.end(); it++) {
			FD_SET(*it, &sockets);
			maxFD = *it > maxFD ? *it : maxFD;
		}
		timeout.tv_sec = 0;
		timeout.tv_usec = 50000;
		int numActiveSocket = select(maxFD + 1, &sockets, NULL, NULL, &timeout);
		if (numActiveSocket == -1) {
			perror("error when listening for actions");
			continue;
		}
		for (std::list<int>::iterator it = clientListCopied.begin(); it
				!= clientListCopied.end(); it++)
			if (FD_ISSET(*it,&sockets))
				recvActionDrop(*it);
	}
}

void Server::recvActionDrop(int clientSocket) {
	char* data[controlDrop.getDropSize()];
	int dataRead = 0;
	while (dataRead < controlDrop.getDropSize()) {
	  int retval = recv(clientSocket, data + dataRead, controlDrop.getDropSize() - dataRead, MSG_WAITALL);
	  if (retval == -1) {
	    perror("error when receiving actions");
	    removeClient(clientSocket);
	    return;
	  }
	  dataRead += retval;
	}
	controlDrop.readArray(data);
	actionBuffer.pushAction(controlDrop);
}

void Server::addClient(int clientSocket) {
	pthread_mutex_lock(&clientListMutex);
	clientList.push_back(clientSocket);
	actionBuffer.setEnable(true);
	pthread_mutex_unlock(&clientListMutex);
}

bool Server::sendDropToClient(int clientSocket, char* stateDrop, int size) {
	int retval = send(clientSocket, stateDrop, size, MSG_NOSIGNAL | MSG_DONTWAIT);
	if (retval < 0) {
		perror("Error when sending to client");
		return (errno == EAGAIN || errno == EWOULDBLOCK);
	}
	return true;
}

void Server::processStateDrop(const CritterStateDrop& stateDrop) {
	pthread_mutex_lock(&clientListMutex);
	std::list<int> clientListCopied = clientList;
	pthread_mutex_unlock(&clientListMutex);
	if (clientListCopied.size() == 0)
		return;
	int dataSize = stateDrop.getDropSize();
	char data[dataSize];
	stateDrop.writeArray(data);
	for (std::list<int>::iterator it = clientListCopied.begin(); it
			!= clientListCopied.end(); it++)
		if (!sendDropToClient(*it, data, dataSize))
			removeClient(*it);
}

void Server::removeClient(int clientSocket) {
	std::cout << "Removing client" << std::endl;
	close(clientSocket);
	pthread_mutex_lock(&clientListMutex);
	clientList.remove(clientSocket);
	if (clientList.size() == 0)
		actionBuffer.setEnable(false);
	pthread_mutex_unlock(&clientListMutex);
}

bool Server::hasClients() {
	pthread_mutex_lock(&clientListMutex);
	int result = clientList.size();
	pthread_mutex_unlock(&clientListMutex);
	return result != 0;
}
