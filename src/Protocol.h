/*
 * Protocol.h
 *
 *  Created on: Feb 13, 2013
 *      Author: Elad Ben Zaken
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_


#define PORT 5555
#define CLIENTS_AMOUNT 512
#define SERVER_IP "127.0.0.1"

#define LOGIN 1
#define LOGIN_FAILED 2
#define LOGIN_SUCCEED 3
#define REGISTER 4
#define REGISTER_FAILED 5
#define REGISTER_SUCCEED 6
#define START_GAME 7
#define FAILED_TO_START_GAME 8
#define AUTO_START_GAME 9
#define NO_AVAILABLE_PLAYER 10
#define GAME_REQUEST 11
#define ACCEPT_GAME_REQUEST 12
#define DECLINE_GAME_REQUEST 13
#define GAME_STARTED 14
#define LIST_OF_USERS 15
#define SCORE_BOARD 16
#define END_GAME 17
#define DISCONNECT 18
#define INVALID_COMMAND 19
#define ALLREADY_LOGGED_IN 20

#define WINNER 100
#define LOSER 101
#define DRAW 102
#define CANCALED 103


#endif /* PROTOCOL_H_ */
