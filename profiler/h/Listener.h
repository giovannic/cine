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
	Listener();
	virtual ~Listener();
	virtual bool listen();
private:
	pthread_t listener;
	int invFifo;
};


#endif /* LISTENER_H_ */
