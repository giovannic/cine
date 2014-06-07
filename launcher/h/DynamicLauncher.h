/*
 * DynamicLauncher.h
 *
 *  Created on: 20 May 2014
 *      Author: giovanni
 */

#ifndef DYNAMICLAUNCHER_H_
#define DYNAMICLAUNCHER_H_

#include "Launcher.h"
#include "Controller.h"
#include "Listener.h"

class DynamicLauncher: public Launcher {
public:
	DynamicLauncher();
	virtual ~DynamicLauncher();
	bool launch();
	bool setup();
private:
	BPatch_process *app;
	Controller *ctrl;
	Listener *listener;
	BPatch_process *createProcess();
	void listenResults();
};

#endif /* DYNAMICLAUNCHER_H_ */
