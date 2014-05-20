/*
 * DynamicLauncher.h
 *
 *  Created on: 20 May 2014
 *      Author: giovanni
 */

#ifndef DYNAMICLAUNCHER_H_
#define DYNAMICLAUNCHER_H_

#include "Launcher.h"

class DynamicLauncher: public Launcher {
public:
	DynamicLauncher(string &input);
	virtual ~DynamicLauncher();
	bool launch();
private:
	BPatch_process *createProcess();
	void listenResults();
	void setup();
};

#endif /* DYNAMICLAUNCHER_H_ */
