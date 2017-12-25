/*
 * LoginAndRegister.cpp
 *
 *  Created on: Aug 30, 2017
 *      Author: elad
 */

#include "LoginAndRegister.h"
#include "MultipleTCPSocketListener.h"
#include "Protocol.h"
#include <fstream>
#define BUFFER_SIZE 256

namespace networkingLab {

LoginAndRegister::LoginAndRegister(LAGHandler* handler) {
	this->handler = handler;
	this->stopLoginAndRegister = true;
}

void LoginAndRegister::run() {
	while (!stopLoginAndRegister) {
		MultipleTCPSocketListener listener;
		listener.addSockets(getGuestsByVector());
		TCPSocket* readyGuestSocket = (TCPSocket*) listener.listenToSocket(1);

		if (readyGuestSocket == NULL)
			continue;

		int cmd = readCommandFromGuest(readyGuestSocket);

		switch (cmd) {
		case LOGIN:
			login(readyGuestSocket);
			break;
		case REGISTER:
			registerr(readyGuestSocket);
			break;
		case DISCONNECT:
			disconnect(readyGuestSocket);
			break;
		default:
			sendCommandToGuest(readyGuestSocket, INVALID_COMMAND);
			break;
		}
	}
}

void LoginAndRegister::add(TCPSocket* connectionSocket) {
	if (connectionSocket != NULL)
		guests[connectionSocket->getConnectionIPandPort()] = connectionSocket;
}

void LoginAndRegister::startt() {
	this->stopLoginAndRegister = false;
	this->start();
	handler->startt();
}

void LoginAndRegister::stop() {
	this->stopLoginAndRegister = true;
	handler->stop();
}

void LoginAndRegister::login(TCPSocket* guestSocket) {

	string userName = readDataFromGuest(guestSocket);
	string password = readDataFromGuest(guestSocket);
	if (handler->isLoggedIn(userName) == true) {
		sendCommandToGuest(guestSocket, ALLREADY_LOGGED_IN);
		sendDataToGuest(guestSocket, userName);
		return;
	}

	password = convertIntToString(stringHashCode(password));

	bool passedFlag = false;
	string logUser;
	string logPass;
	string logScore;
	int score;

	ifstream input;
	input.open("usersANDpasswords.txt");

	if (input.is_open()) {
		while (!input.eof()) {
			input >> logUser;
			input >> logPass;
			input >> logScore;

			if (userName == logUser && password == logPass) {
				passedFlag = true;
				sscanf(logScore.c_str(), "%d", &score);
				break;
			}
		}
	}
	input.close();
	if (passedFlag) {
		guests.erase(guestSocket->getConnectionIPandPort());
		cout << userName << " passed from Login and Registration to the Lobby"
				<< endl;
		handler->add(new UserData(guestSocket, userName, password, score));
		sendCommandToGuest(guestSocket, LOGIN_SUCCEED);
		sendDataToGuest(guestSocket, userName);
	} else
		sendCommandToGuest(guestSocket, LOGIN_FAILED);

}

void LoginAndRegister::registerr(TCPSocket* guestSocket) {

	string userName = readDataFromGuest(guestSocket);
	string password = readDataFromGuest(guestSocket);

	string logUser;
	string logPass;
	string logScore;

	ifstream input;
	input.open("usersANDpasswords.txt");
	if (input.is_open()) {
		while (!input.eof()) {
			input >> logUser;
			input >> logPass;
			input >> logScore;
			if (userName == logUser) {  // can not register this user
				sendCommandToGuest(guestSocket, REGISTER_FAILED);
				return;
			}
		}
	}
	input.close();

	// register the user in the system
	ofstream output;
	output.open("usersANDpasswords.txt", fstream::app);

	password = convertIntToString(stringHashCode(password));

	output << userName << endl;
	output << password << endl;
	output << "0" << endl;

	guests.erase(guestSocket->getConnectionIPandPort());
	cout << userName << " logged in" << endl;
	handler->add(new UserData(guestSocket, userName, password, 0));
	sendCommandToGuest(guestSocket, REGISTER_SUCCEED);
	sendCommandToGuest(guestSocket, LOGIN_SUCCEED);
	sendDataToGuest(guestSocket, userName);
}

void LoginAndRegister::disconnect(TCPSocket* guestSocket) {
	guests.erase(guestSocket->getConnectionIPandPort());
	sendCommandToGuest(guestSocket, DISCONNECT);
}

int LoginAndRegister::readCommandFromGuest(TCPSocket* guest) {
	int command = 0;
	guest->readd((char*) &command, sizeof(int));
	return ntohl(command);
}

string LoginAndRegister::readDataFromGuest(TCPSocket* guest) {
	int dataSize = 0;
	guest->readd((char*) &dataSize, sizeof(int));
	dataSize = ntohl(dataSize);
	char buffer[BUFFER_SIZE];

	int rd = guest->readd(buffer, dataSize);
	buffer[rd] = '\0';
	string msg = buffer;
	return msg;
}

void LoginAndRegister::sendCommandToGuest(TCPSocket* guest, int cmd) {
	int command = htonl(cmd);
	guest->writee((char*) &command, sizeof(int));
}

void LoginAndRegister::sendDataToGuest(TCPSocket* guest, string msg) {
	int len = htonl(msg.length());
	guest->writee((char*) &len, sizeof(int));
	guest->writee(msg.c_str(), msg.length() * sizeof(char));
}

vector<IFile*> LoginAndRegister::getGuestsByVector() {
	map<string, IFile*>::iterator begin = guests.begin();
	map<string, IFile*>::iterator end = guests.end();
	vector<IFile*> vector;
	while (begin != end)
		vector.push_back((begin++)->second);
	return vector;
}

int LoginAndRegister::stringHashCode(string str) {
	int seed = 131;
	unsigned long hash = 0;
	int len = str.length();
	for (int i = 0; i < len; i++)
		hash = (hash * seed) + str[i];
	return hash % 6700417;
}

string LoginAndRegister::convertIntToString(int num) {
	char strr[15];
	sprintf(strr, "%d", num);
	string strrr = (string) (strr);
	return strrr;
}

LoginAndRegister::~LoginAndRegister() {
}

} /* namespace networkingLab */

