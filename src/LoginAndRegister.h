/*
 * LoginAndRegister.h
 *
 *  Created on: Aug 30, 2017
 *      Author: elad
 */

#ifndef LOGINANDREGISTER_H_
#define LOGINANDREGISTER_H_
#include "Lobby.h"
#include <string>
#include <iostream>
#include "UserData.h"
#include "MThread.h"
#include "LAGHandler.h"
#include "ServerHandler.h"
#include <map>

using namespace std;

namespace networkingLab {

/**
 * login and registration section
 */
class LoginAndRegister: public MThread, public ServerHandler {
private:
	LAGHandler* handler;
	volatile bool stopLoginAndRegister;
	map<string, IFile*> guests; // guests holder

	/**
	 * Existing user can login to the system with his username and password
	 */
	void login(TCPSocket* guestSocket);

	/**
	 * New user can register to the system with a username and password
	 */
	void registerr(TCPSocket* guestSocket);

	/**
	 * Disconnect from login and registration section
	 */
	void disconnect(TCPSocket* guestSocket);


	/**
	 * Communication methods
	 */
	int readCommandFromGuest(TCPSocket* guest);
	string readDataFromGuest(TCPSocket* guest);
	void sendCommandToGuest(TCPSocket* guest, int cmd);
	void sendDataToGuest(TCPSocket* guest, string msg);

	/**
	 * Class functional methods
	 */
	int stringHashCode(string str);
	string convertIntToString(int num);
	vector<IFile*> getGuestsByVector();


public:
	LoginAndRegister(LAGHandler* handler); // c'tor

	/**
	 * Add new guest to login and registration section
	 */
	void add(TCPSocket* connectionSocket);

	/**
	 * Start login and registration thread
	 */
	void startt();

	/**
	 * Stop login and registration thread
	 */
	void stop();

	void run(); // MThread

	virtual ~LoginAndRegister(); // d'tor
};

} /* namespace networkingLab */

#endif /* LOGINANDREGISTER_H_ */
