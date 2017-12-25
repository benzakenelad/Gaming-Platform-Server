/*
 * Server.h
 *
 *  Created on: Aug 30, 2017
 *      Author: elad
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "LoginAndRegister.h"
#include "MThread.h"
#include "TCPSocket.h"
#include "ServerHandler.h"

namespace networkingLab {

class Server : public MThread{
private:
	ServerHandler* handler; // Login and Registration management
	TCPSocket* listeningSocket; // Server listening socket
	volatile bool stopServer;


public:
	Server(ServerHandler* handler); // c'tor

	/**
	 * Start server thread
	 */
	void startt();

	/**
	 * stop server thread
	 */
	void stop();

	void run(); // MThread

	virtual ~Server(); // d'tor
};

} /* namespace networkingLab */

#endif /* SERVER_H_ */
