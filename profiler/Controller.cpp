/*
 * Controller.cpp
 *
 *  Created on: 29 Apr 2014
 *      Author: giovanni
 */

#include "Controller.h"

#include "callbacks.h"

#include "PCProcess.h"
#include "Event.h"

#include "VTF.h"

using namespace Dyninst::ProcControlAPI;
using namespace VEX;

BPatch_thread *Controller::getEventThread(Event::const_ptr e){
	BPatch_thread *t = proc->getThread((dynthread_t) e->getEventNewThread()->getNewThread()->getTID());
	if(t == NULL){
		cerr << "could not find thread new thread " << e->getThread()->getTID() << endl;
	}
	return t;
}

bool Controller::registerThreadStart(Dyninst::Address startAddress){
	return inst->instrumentThreadEntry(startAddress);
}

bool Controller::registerThreadStart(BPatch_thread *thread){
	return inst->instrumentThreadEntry(proc, thread);
}

void Controller::listenThreads(){
	Process::registerEventCallback(EventType::UserThreadCreate, on_thread_create);
//	Process::registerEventCallback(EventType::UserThreadDestroy, on_thread_exit);
//	Process::registerEventCallback(EventType::LWPCreate, on_thread_create);
//	Process::registerEventCallback(EventType::LWPDestroy, on_thread_exit);
}

Controller::Controller(Instrumenter *inst, Analyser *a, BPatch_process *proc){
	ctrl = this;
	this->proc = proc;
	this->inst = inst;
	this->analyser = a;
}

Controller::~Controller() {
}

void Controller::listenResults() {
	Process::registerEventCallback(EventType::Terminate, cine_on_exit);
}

void Controller::getResults() {

	//change to be nicer
	BPatch_constExpr directory("/tmp");
	BPatch_function *print = analyser->getFunction("VEX::printResults");

	vector<BPatch_snippet *>args;
	args.push_back(&directory);
	BPatch_funcCallExpr printCall(*print, args);

	bool err;
	this->proc->oneTimeCode(printCall, &err);

	if(err){
		cerr << "some onetime code error on exit " << endl;
	}

}
