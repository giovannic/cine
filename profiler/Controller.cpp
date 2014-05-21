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

Controller::Controller(Instrumenter *inst, Analyser *a, BPatch_process *proc){
	ctrl = this;
	this->proc = proc;
	this->inst = inst;
	this->analyser = a;
}

Controller::~Controller() {
}

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



void Controller::listenResults() {
	Process::registerEventCallback(EventType::Terminate, cine_on_exit);
}

bool Controller::beginSimulator(){
	BPatch_function * init = analyser->getFunction("cine_init");
	vector<BPatch_snippet *> args;
	BPatch_nullExpr null;
	args.push_back(&null);
	BPatch_funcCallExpr initCall(*init, args);
	proc->oneTimeCode(initCall);
	return true;
}

bool Controller::registerMethods(){
	vector<BPatch_function *>fs;
	analyser->getUsefulFunctions(fs);

	BPatch_function *reg = analyser->getFunction("cine_method_registration");

	vector<BPatch_function *> ms;
	analyser->getUsefulFunctions(ms);
	int mid = 0;
	vector<BPatch_snippet *> registrations;
	for(vector<BPatch_function *>::const_iterator mi = ms.begin();
			mi != ms.end(); mi++){
		bool err;
		BPatch_function *f = *mi;
		vector<BPatch_snippet *> margs;
		const char *n = f->getName().c_str();
		BPatch_constExpr name(n);
		margs.push_back(&name);
		BPatch_constExpr id(mid);
		margs.push_back(&id);

		BPatch_funcCallExpr regCall(*reg, margs);
		proc->oneTimeCode(regCall, &err);
		if(err){
			return false;
		}

		inst->timeFunction(f, mid);
//		inst->timeFunctionCalls(f, mid);

		mid++;
	}
	return true;
}

void Controller::getResults() {

	//change to be nicer
	BPatch_constExpr directory("/tmp");
	BPatch_function *print = analyser->getFunction("cine_get_results");

	vector<BPatch_snippet *>args;
	args.push_back(&directory);
	BPatch_funcCallExpr printCall(*print, args);

	bool err;
	this->proc->oneTimeCode(printCall, &err);

	if(err){
		cerr << "some onetime code error on exit " << endl;
	}
}
