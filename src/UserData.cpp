/*
 * UserData.cpp
 *
 *  Created on: Aug 30, 2017
 *      Author: elad
 */

#include "UserData.h"

namespace networkingLab {

UserData::UserData(TCPSocket* connectionSocket, string userName, string passWord, int score) {
	this->connectionSocket = connectionSocket;
	this->userName = userName;
	this->score = score;
	this->passWord = passWord;
}

TCPSocket* UserData::getConnectionSocket() {
	return connectionSocket;
}

string UserData::getUserName() {
	return userName;
}

int UserData::getScore() {
	return score;
}

string UserData::getPassWord(){
	return passWord;
}

void UserData::setScore(int newScore) {
	this->score = newScore;
}

UserData::~UserData() {
}

} /* namespace networkingLab */
