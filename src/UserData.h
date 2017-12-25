/*
 * UserData.h
 *
 *  Created on: Aug 30, 2017
 *      Author: elad
 */

#ifndef USERDATA_H_
#define USERDATA_H_

#include <iostream>
#include <string>
#include <TCPSocket.h>

using namespace std;

namespace networkingLab {

/**
 * User's data holder
 */
class UserData {
private:
	TCPSocket* connectionSocket;
	string userName;
	string passWord;
	int score;
public:
	UserData(TCPSocket* connectionSocket, string userName, string passWord, int score); // c'tor

	/**
	 * getters and setters
	 */
	TCPSocket* getConnectionSocket();
	string getUserName();
	int getScore();
	string getPassWord();
	void setScore(int newScore);

	virtual ~UserData(); // d'tor
};

} /* namespace networkingLab */

#endif /* USERDATA_H_ */
