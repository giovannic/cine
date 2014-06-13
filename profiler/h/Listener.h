/*
 * Listener.h
 *
 *  Created on: 2 Jun 2014
 *      Author: giovanni
 */

#ifndef LISTENER_H_
#define LISTENER_H_

#include "pthread.h"

class Listener {
public:
	Listener(Instrumenter *inst);
	virtual ~Listener();
	virtual bool listen();
private:
	pthread_t listener;
	int invFifo;
	Instrumenter *inst;
};


#endif /* LISTENER_H_ */
