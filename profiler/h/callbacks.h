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
#include "BPatch_snippet.h"
#include "BPatch_process.h"
#include "BPatch_thread.h"

using namespace std;
using namespace Dyninst::ProcControlAPI;

BPatch_function *cb_create_function;
BPatch_function *cb_exit_function;

BPatch_process *cb_main_proc;

//TODO:make this execute on the target with irpc or callback delivery
Process::cb_ret_t on_thread_create(Event::const_ptr ev) {

//	Thread::const_ptr t = ev->getThread();
//	long id = t->getTID();
//
//	if (t->isInitialThread()){
//		threadEventsBehaviour->onThreadMainStart(id);
//	} else {
//		threadEventsBehaviour->onStart(id, "subthread");
//	}

	cout <<"listener create" << endl;
	vector<BPatch_snippet *>args;
	BPatch_funcCallExpr call(*cb_create_function, args);

	//get bpatch thread
	BPatch_thread *t = cb_main_proc->getThread((dynthread_t) ev->getThread()->getTID());

	cout << "searching for " << ev->getThread()->getTID() << endl;

	vector<BPatch_thread *>ts;
	cb_main_proc->getThreads(ts);
	for (vector<BPatch_thread *>::iterator it = ts.begin();
			it != ts.end(); it++){
		BPatch_thread *ti = *it;
		cout << ti->getTid() << endl;
	}

	if(t == NULL){
		cerr << "could not find thread new thread " << ev->getThread()->getTID() << endl;
		return Process::cbDefault;
	}

	bool error;
	t->oneTimeCode(call, &error);
	if (error){
		cerr << "error executing one time code" << endl;
		return Process::cbProcStop;
	}
	return Process::cbDefault;

}

Process::cb_ret_t on_thread_exit(Event::const_ptr ev) {
	cout <<"listener exit" << endl;
	return Process::cbDefault;

}

#endif /* CALLBACKS_H_ */
