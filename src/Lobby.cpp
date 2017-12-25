/*
 * Lobby.cpp
 *
 *  Created on: Aug 30, 2017
 *      Author: elad
 */

#include "Lobby.h"
#include "MultipleTCPSocketListener.h"
#include "Protocol.h"
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <vector>

#define BUFFER_SIZEE 256

namespace networkingLab {

Lobby::Lobby() {
	stopLobby = true;
}

void Lobby::add(UserData* user) {
	waitingUsers[user->getUserName()] = user;
}

void Lobby::startt() {
	stopLobby = false;
	this->start();
}

void Lobby::stop() {
	stopLobby = true;
}

void Lobby::run() {
	while (!stopLobby) {
		MultipleTCPSocketListener listener;
		listener.addSockets(getAllPlayersByVector());
		TCPSocket* readyPlayerSocket = (TCPSocket*) listener.listenToSocket(1);

		if (readyPlayerSocket == NULL)
			continue;

		int cmd = readCommandFromPlayer(readyPlayerSocket);

		switch (cmd) {
		case START_GAME:
			startGame(readyPlayerSocket);
			break;
		case AUTO_START_GAME:
			autoStartGame(readyPlayerSocket);
			break;
		case ACCEPT_GAME_REQUEST:
			acceptGame(readyPlayerSocket);
			break;
		case DECLINE_GAME_REQUEST:
			declineGame(readyPlayerSocket);
			break;
		case END_GAME:
			endGame(readyPlayerSocket);
			break;
		case DISCONNECT:
			disconncet(readyPlayerSocket);
			break;
		case SCORE_BOARD:
			sendScoreBoard(readyPlayerSocket);
			break;
		case LIST_OF_USERS:
			sendListOfUsers(readyPlayerSocket);
			break;
		case LOGIN:
			sendCommandToPlayer(readyPlayerSocket, ALLREADY_LOGGED_IN);
			break;
		case REGISTER:
			sendCommandToPlayer(readyPlayerSocket, ALLREADY_LOGGED_IN);
			break;
		default:
			sendCommandToPlayer(readyPlayerSocket, INVALID_COMMAND);
			break;
		}

	}
}

void Lobby::startGame(TCPSocket* player) {
	// the promoter user send first his username and then the other username
	string user1Name = readDataFromPlayer(player);
	string user2Name = readDataFromPlayer(player);
	UserData* user1Data = waitingUsers[user1Name];
	UserData* user2Data = waitingUsers[user2Name];
	if (user1Data == NULL || user2Data == NULL) {
		sendCommandToPlayer(player, FAILED_TO_START_GAME);
		return;
	}
	if (user1Name == user2Name) {
		sendCommandToPlayer(player, FAILED_TO_START_GAME);
		return;
	}
	// users are exist and waiting
	// send to user 2 game request + user 1 user name
	sendCommandToPlayer(user2Data->getConnectionSocket(), GAME_REQUEST);
	sendDataToPlayer(user2Data->getConnectionSocket(), user1Name);
}

void Lobby::autoStartGame(TCPSocket* player) {
	// the promoter user send first his username
	string user1Name = readDataFromPlayer(player);
	UserData* user1Data = waitingUsers[user1Name];
	if (user1Data == NULL) {
		sendCommandToPlayer(player, FAILED_TO_START_GAME);
		return;
	}
	// user is exist and waiting
	// find a partner
	map<string, UserData*>::iterator begin = waitingUsers.begin();
	map<string, UserData*>::iterator end = waitingUsers.end();
	while (begin != end) {
		if (begin->second->getUserName() != user1Name)
			break;
		begin++;
	}
	if (begin == end) {
		sendCommandToPlayer(player, NO_AVAILABLE_PLAYER);
		return;
	}
	// send to the matching user game request + user 1 user name
	sendCommandToPlayer(begin->second->getConnectionSocket(), GAME_REQUEST);
	sendDataToPlayer(begin->second->getConnectionSocket(), user1Name);
}

void Lobby::acceptGame(TCPSocket* player) {
	string user1Name = readDataFromPlayer(player);
	string user2Name = readDataFromPlayer(player);
	UserData* user1Data = waitingUsers[user1Name];
	UserData* user2Data = waitingUsers[user2Name];
	if (user1Data == NULL || user2Data == NULL) {
		sendCommandToPlayer(player, FAILED_TO_START_GAME);
		return;
	}

	sendCommandToPlayer(user2Data->getConnectionSocket(), GAME_STARTED);
	sendDataToPlayer(user2Data->getConnectionSocket(), user1Name);
	sendCommandToPlayer(user1Data->getConnectionSocket(), GAME_STARTED);
	sendDataToPlayer(user1Data->getConnectionSocket(), user2Name);

	srand(time(NULL));   // should only be called once
	int r = rand();
	if (r < 0)
		r *= (-1);
	r = r % 2;

	// send opponent IP and port
	sendDataToPlayer(user2Data->getConnectionSocket(),
			user1Data->getConnectionSocket()->getIP());
	sendDataToPlayer(user1Data->getConnectionSocket(),
			user2Data->getConnectionSocket()->getIP());
	sendCommandToPlayer(user2Data->getConnectionSocket(),
			user1Data->getConnectionSocket()->getPort());
	sendCommandToPlayer(user1Data->getConnectionSocket(),
			user2Data->getConnectionSocket()->getPort());

	// send who start the game

	if (r == 0) {
		sendCommandToPlayer(user2Data->getConnectionSocket(), 0);
		sendCommandToPlayer(user1Data->getConnectionSocket(), 1);
	} else {
		sendCommandToPlayer(user2Data->getConnectionSocket(), 1);
		sendCommandToPlayer(user1Data->getConnectionSocket(), 0);
	}

	cout << "A match started between " << user1Name << " and " << user2Name
			<< endl;
	waitingUsers.erase(user1Name);
	waitingUsers.erase(user2Name);
	playingUsers[user1Name] = user1Data;
	playingUsers[user2Name] = user2Data;
}

void Lobby::declineGame(TCPSocket* player) {
	// user1Name = the ignoring player, user2Name = the ignored player
	string user1Name = readDataFromPlayer(player);
	string user2Name = readDataFromPlayer(player);
	UserData* user1Data = waitingUsers[user1Name];
	UserData* user2Data = waitingUsers[user2Name];
	if (user1Data == NULL || user2Data == NULL) {
		sendCommandToPlayer(player, FAILED_TO_START_GAME);
		return;
	}
	sendCommandToPlayer(user2Data->getConnectionSocket(), DECLINE_GAME_REQUEST);
	sendDataToPlayer(user2Data->getConnectionSocket(), user1Name);
}

void Lobby::endGame(TCPSocket* player) {
	// first reading the users name
	string user1Name = readDataFromPlayer(player);
	string user2Name = readDataFromPlayer(player);
	UserData* user1Data = playingUsers[user1Name];
	UserData* user2Data = playingUsers[user2Name];
	if (user1Data == NULL || user2Data == NULL) {
		cout << "ERROR IN GAME END METHOD" << endl;
		sendCommandToPlayer(player, INVALID_COMMAND);
		return;
	}
	// users located
	// updating the score
	string winner = "";
	int score = readCommandFromPlayer(player);
	int points = readCommandFromPlayer(player);
	if (score == WINNER) {
		user1Data->setScore(user1Data->getScore() + points);
		winner = user1Name;
		updateUserData(user1Data);
	} else if (score == LOSER) {
		user2Data->setScore(user2Data->getScore() + points);
		winner = user2Name;
		updateUserData(user2Data);
	}

	sendCommandToPlayer(user2Data->getConnectionSocket(), END_GAME);
	sendDataToPlayer(user2Data->getConnectionSocket(), user1Name);
	sendDataToPlayer(user2Data->getConnectionSocket(), user2Name);
	sendCommandToPlayer(user2Data->getConnectionSocket(), score);
	sendCommandToPlayer(user2Data->getConnectionSocket(), points);
	sendCommandToPlayer(user1Data->getConnectionSocket(), END_GAME);
	sendDataToPlayer(user1Data->getConnectionSocket(), user1Name);
	sendDataToPlayer(user1Data->getConnectionSocket(), user2Name);
	sendCommandToPlayer(user1Data->getConnectionSocket(), score);
	sendCommandToPlayer(user1Data->getConnectionSocket(), points);

	playingUsers.erase(user1Name);
	playingUsers.erase(user2Name);
	waitingUsers[user1Name] = user1Data;
	waitingUsers[user2Name] = user2Data;
}

void Lobby::sendListOfUsers(TCPSocket* waitingPlayer) {
	map<string, UserData*>::iterator begin = waitingUsers.begin();
	map<string, UserData*>::iterator end = waitingUsers.end();
	vector<string> userNames;
	while (begin != end)
		userNames.push_back((begin++)->second->getUserName());
	int size = userNames.size();
	if (size <= 0)
		return;
	sendCommandToPlayer(waitingPlayer, LIST_OF_USERS); // send command
	sendCommandToPlayer(waitingPlayer, size); // send number of users
	vector<string>::iterator beginn = userNames.begin();
	vector<string>::iterator endd = userNames.end();
	while (beginn != endd)
		sendDataToPlayer(waitingPlayer, (beginn++)->data()); // sending every username
}

void Lobby::sendScoreBoard(TCPSocket* player) {
	string logUser;
	string logPass;
	string logScore;
	int score = 0;
	vector<UserData> vectorr;

	ifstream input;
	input.open("usersANDpasswords.txt");

	if (input.is_open()) // counting 1 extra
		while (!input.eof()) {
			input >> logUser;
			input >> logPass;
			input >> logScore;
			score = convertStringToInt(logScore);
			UserData newUser(NULL, logUser, logPass, score);
			vectorr.push_back(newUser);
		}

	int size = vectorr.size() - 1;

	for (int i = 0; i < (size - 1); i++) {
		for (int j = 0; j < size - i - 1; j++) {
			if (vectorr[j].getScore() < vectorr[j + 1].getScore()) {
				UserData temp(NULL, "", "", 0);
				temp = vectorr[j];
				vectorr[j] = vectorr[j + 1];
				vectorr[j + 1] = temp;
			}
		}
	}
	sendCommandToPlayer(player, SCORE_BOARD);
	sendCommandToPlayer(player, size);
	for (int i = 0; i < size; i++) {
		sendDataToPlayer(player, vectorr[i].getUserName());
		sendCommandToPlayer(player, vectorr[i].getScore());
	}

}

void Lobby::disconncet(TCPSocket* player) {
	string userName = readDataFromPlayer(player);
	UserData* userData = waitingUsers[userName];

	if (userData == NULL) {
		cout << "ERROR IN DISCONNECT METHOD" << endl;
		return;
	}
	// updateUserData(userData);
	sendCommandToPlayer(player, DISCONNECT);
	waitingUsers.erase(userName);
	cout << "userName " << "disconnected from server" << endl;
}

void Lobby::updateUserData(UserData* user) {

	string logUser;
	string logPass;
	string logScore;
	int score = 0;
	vector<UserData> vectorr;

	ifstream input;
	input.open("usersANDpasswords.txt");

	if (input.is_open()) // counting 1 extra
		while (!input.eof()) {
			input >> logUser;
			input >> logPass;
			input >> logScore;
			score = convertStringToInt(logScore);
			UserData newUser(NULL, logUser, logPass, score);
			vectorr.push_back(newUser);
		}

	int size = vectorr.size() - 1;
	for (int i = 0; i < size; i++)
		if (vectorr[i].getUserName() == user->getUserName()) {
			vectorr[i].setScore(user->getScore());
			break;
		}

	input.close();

	ofstream output;
	output.open("usersANDpasswords.txt");

	for (int i = 0; i < size; i++) {
		output << vectorr[i].getUserName() << endl;
		output << vectorr[i].getPassWord() << endl;
		output << vectorr[i].getScore() << endl;
	}

	output.close();
	return;
}

vector<IFile*> Lobby::getWaitingPlayersByVector() {
	map<string, UserData*>::iterator begin = waitingUsers.begin();
	map<string, UserData*>::iterator end = waitingUsers.end();
	vector<IFile*> vector;
	while (begin != end)
		vector.push_back((begin++)->second->getConnectionSocket());
	return vector;
}

vector<IFile*> Lobby::getAllPlayersByVector() {
	map<string, UserData*>::iterator begin = playingUsers.begin();
	map<string, UserData*>::iterator end = playingUsers.end();
	map<string, UserData*>::iterator beginn = waitingUsers.begin();
	map<string, UserData*>::iterator endd = waitingUsers.end();
	vector<IFile*> vector;
	while (begin != end)
		vector.push_back((begin++)->second->getConnectionSocket());
	while (beginn != endd)
		vector.push_back((beginn++)->second->getConnectionSocket());
	return vector;
}

bool Lobby::isLoggedIn(string userName) {
	map<string, UserData*>::iterator begin = waitingUsers.begin();
	map<string, UserData*>::iterator end = waitingUsers.end();
	while (begin != end)
		if ((begin++)->second->getUserName() == userName)
			return true;
	map<string, UserData*>::iterator beginn = playingUsers.begin();
	map<string, UserData*>::iterator endd = playingUsers.end();
	while (beginn != endd)
		if ((beginn++)->second->getUserName() == userName)
			return true;
	return false;
}

int Lobby::readCommandFromPlayer(TCPSocket* player) {
	int command = 0;
	player->readd((char*) &command, sizeof(int));
	return ntohl(command);
}

string Lobby::readDataFromPlayer(TCPSocket* player) {
	int dataSize = 0;
	player->readd((char*) &dataSize, sizeof(int));
	dataSize = ntohl(dataSize);
	char buffer[BUFFER_SIZEE];

	int rd = player->readd(buffer, dataSize);
	buffer[rd] = '\0';
	string msg = buffer;
	return msg;
}

void Lobby::sendCommandToPlayer(TCPSocket* client, int cmd) {
	int command = htonl(cmd);
	client->writee((char*) &command, sizeof(int));
}

void Lobby::sendDataToPlayer(TCPSocket* client, string msg) {
	int len = htonl(msg.length());
	client->writee((char*) &len, sizeof(int));
	client->writee(msg.c_str(), msg.length() * sizeof(char));
}

int Lobby::convertStringToInt(string str) {
	int num = 0;
	sscanf(str.c_str(), "%d", &num);
	return num;
}

Lobby::~Lobby() {
}

} /* namespace networkingLab */
