/*
 * Server.cpp
 *
 *  Created on: Aug 30, 2017
 *      Author: elad
 */

#include "Server.h"
#include "Protocol.h"

namespace networkingLab {

Server::Server(ServerHandler* handler) {
	this->handler = handler;
	listeningSocket = new TCPSocket(PORT);

}

void Server::run() {
	listeningSocket->listenn(CLIENTS_AMOUNT);
	while(!stopServer){
		TCPSocket* connectionSocket = listeningSocket->acceptt();
		cout << connectionSocket->getConnectionIPandPort() << " is connected to server." << endl;
		handler->add(connectionSocket);
	}
}

void Server::startt() {
	stopServer = false;
	this->start();
	handler->startt();
}

void Server::stop() {
	stopServer = true;
	handler->stop();
}

Server::~Server() {
}

} /* namespace networkingLab */
