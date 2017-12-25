/*
 * Lobby.h
 *
 *  Created on: Aug 30, 2017
 *      Author: elad
 */

#ifndef LOBBY_H_
#define LOBBY_H_
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include "UserData.h"
#include "MThread.h"
#include "LAGHandler.h"


using namespace std;
namespace networkingLab {

class Lobby : public MThread, public LAGHandler{
private:
	map<string, UserData*> playingUsers; // playing users hash map
	map<string, UserData*> waitingUsers; // waiting for game users hash map
	volatile bool stopLobby;

	/**
	 * Communication methods
	 */
	int readCommandFromPlayer(TCPSocket* player);
	string readDataFromPlayer(TCPSocket* player);
	void sendCommandToPlayer(TCPSocket* client, int cmd);
	void sendDataToPlayer(TCPSocket* client, string msg);

	/**
	 * Functional methods
	 */
	vector<IFile*> getWaitingPlayersByVector();
	vector<IFile*> getAllPlayersByVector();
	int convertStringToInt(string str);
	void updateUserData(UserData* user);


public:
	Lobby();

	/**
	 * Start lobby section
	 */
	void startt();

	/**
	 * Stop lobby section
	 */
	void stop();

	/**
	 * Add new user to the lobby (after he logged in)
	 */
	void add(UserData* user);


	/**
	 * A user can send a request to start a game to another user
	 */
	void startGame(TCPSocket* player);

	/**
	 * A user can automatically start a game with a random available user
	 */
	void autoStartGame(TCPSocket* player);

	/**
	 * A user can accept a game request
	 */
	void acceptGame(TCPSocket* player);

	/**
	 * A user can reject a game request
	 */
	void declineGame(TCPSocket* player);

	/**
	 * The game application can end the game and declare its result
	 */
	void endGame(TCPSocket* player);

	/**
	 * User can see the other users which are online and available to play with him
	 */
	void sendListOfUsers(TCPSocket* waitingPlayer);

	/**
	 * After the game a user can see the score board and his position in
	 * it. The score board will order the users according to their accumulating score.
	 */
	void sendScoreBoard(TCPSocket* player);

	/**
	 * Disconnect from server
	 */
	void disconncet(TCPSocket* player);

	/**
	 * Checks if a user is logged in
	 */
	bool isLoggedIn(string userName);

	void run(); // MThread

	virtual ~Lobby(); // d'tor
};

} /* namespace networkingLab */

#endif /* LOBBY_H_ */
