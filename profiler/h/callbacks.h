/*
 * callbacks.h
 *
 *  Created on: 29 Apr 2014
 *      Author: giovanni
 */

#ifndef CALLBACKS_H_
#define CALLBACKS_H_

#include <iostream>

#include "PCProcess.h"
#include "Event.h"
#include "BPatch_process.h"
#include "BPatch_thread.h"

using namespace std;
using namespace Dyninst::ProcControlAPI;

Controller *ctrl;

//TODO:make this execute on the target with irpc or callback delivery
Process::cb_ret_t on_thread_create(Event::const_ptr ev) {

	//cout << ev->getThread()->getStartFunction() << endl;
	ctrl->registerThreadStart(ev->getEventNewUserThread()->getNewThread()->getStartFunction());
	//get bpatch thread
	//BPatch_thread *t = ctrl->getEventThread(ev);
	//ctrl->registerThreadStart(t);

	/*
	t->oneTimeCode(call, &error);
	if (error){
		cerr << "error executing one time code" << endl;
		return Process::cbProcStop;
	}
	*/
	return Process::cbDefault;

}

Process::cb_ret_t cine_on_exit(Event::const_ptr ev) {
	if (ev->getThread())
		cout << ev->getThread()->getTID() << " exited" << endl;
//	ctrl->getResults();
	return Process::cbDefault;
}

#endif /* CALLBACKS_H_ */
