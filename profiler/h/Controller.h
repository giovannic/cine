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
#include "Listener.h"
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
	bool registerInvalidation(string &policy);
	bool beginSimulator();
	bool registerMethods(bool invalidating);
	void cleanupListener();
private:
	void listenInvalidation();
	Instrumenter *inst;
	BPatch_process *proc;
	Analyser *analyser;
	Listener *listener;
};

#endif /* CONTROLLER_H_ */
