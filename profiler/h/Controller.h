/*
 * Controller.h
 *
 *  Created on: 29 Apr 2014
 *      Author: giovanni
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "Instrumenter.h"
#include "Analyser.h"
#include "BPatch_process.h"
#include "Event.h"

class Controller {
public:
	Controller(Instrumenter *inst, Analyser *a, BPatch_process *proc);
	virtual ~Controller();
	void listenThreads();
	void listenResults();
	void getResults();
	BPatch_thread *getEventThread(Dyninst::ProcControlAPI::Event::const_ptr ev);
	bool registerThreadStart(BPatch_thread *t);
	bool registerThreadStart(Dyninst::Address a);
	bool beginSimulator();
	bool registerMethods();
private:
	Instrumenter *inst;
	BPatch_process *proc;
	Analyser *analyser;
};

#endif /* CONTROLLER_H_ */
