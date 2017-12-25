//============================================================================
// Name        : ProjectServer.cpp
// Author      : Elad Ben Zaken
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "Lobby.h"
#include "LoginAndRegister.h"
#include "Server.h"

using namespace std;
using namespace networkingLab;

int main() {

	Server* server = new Server(new LoginAndRegister(new Lobby()));

	cout << "Enter 1 to start server" << endl;
	cout << "Enter 2 to stop server" << endl;
	int command = 0;
	while (true) {
		cin >> command;
		if (command == 1) {
			server->startt();
		} else if (command == 2) {
			server->stop();
			break;
		}  else {
			cout << "Invalid command" << endl;
		}
	}

	return 0;
}
