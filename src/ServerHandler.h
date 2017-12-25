/*
 * ServerHandler.h
 *
 *  Created on: Sep 4, 2017
 *      Author: elad
 */

#ifndef SERVERHANDLER_H_
#define SERVERHANDLER_H_

namespace networkingLab {

class ServerHandler {
public:
	ServerHandler(){}
	virtual void add(TCPSocket* connectionSocket) =0;
	virtual void startt() =0;
	virtual void stop() =0;
	virtual ~ServerHandler(){}
};

} /* namespace networkingLab */

#endif /* SERVERHANDLER_H_ */
