/*
 * LAGHandler.h
 *
 *  Created on: Sep 4, 2017
 *      Author: elad
 */

#ifndef LAGHANDLER_H_
#define LAGHANDLER_H_

namespace networkingLab {

class LAGHandler {
public:
	LAGHandler(){}
	virtual void add(UserData* user) =0;
	virtual void startt() =0;
	virtual void stop() =0;
	virtual bool isLoggedIn(string userName) =0;
	virtual ~LAGHandler(){}
};

} /* namespace networkingLab */

#endif /* LAGHANDLER_H_ */
