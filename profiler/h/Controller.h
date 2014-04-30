/*
 * Controller.h
 *
 *  Created on: 29 Apr 2014
 *      Author: giovanni
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "Analyser.h"
#include "BPatch_process.h"

class Controller {
public:
	Controller(Analyser *a, BPatch_process *proc);
	virtual ~Controller();
	void listenThreads();
private:
};

#endif /* CONTROLLER_H_ */
