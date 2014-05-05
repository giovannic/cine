/*
 * Controller.h
 *
 *  Created on: 29 Apr 2014
 *      Author: giovanni
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "Instrumenter.h"
#include "BPatch_process.h"
#include "Event.h"

class Controller {
public:
	Controller(Instrumenter *inst, BPatch_process *proc);
	virtual ~Controller();
	void listenThreads();
	BPatch_thread *getEventThread(Dyninst::ProcControlAPI::Event::const_ptr ev);
	bool registerThreadStart(BPatch_thread *t);
	bool registerThreadStart(Dyninst::Address a);
private:
	Instrumenter *inst;
	BPatch_process *proc;
};

#endif /* CONTROLLER_H_ */
