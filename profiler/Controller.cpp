/*
 * Controller.cpp
 *
 *  Created on: 29 Apr 2014
 *      Author: giovanni
 */

#include "Controller.h"
#include "AsyncListener.h"

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
	listener = NULL;
}

Controller::~Controller() {
	if(listener != NULL){
		delete listener;
	}
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
	Process::registerEventCallback(EventType::UserThreadDestroy, cine_on_exit);
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

bool Controller::registerInvalidation(string &policy){

	BPatch_function *reg = analyser->getFunction("cine_invalidation_registration");

	bool err;
	vector<BPatch_snippet *> margs;
	const char *n = policy.c_str();
	BPatch_constExpr name(n);
	margs.push_back(&name);

	BPatch_funcCallExpr regCall(*reg, margs);
	proc->oneTimeCode(regCall, &err);
	if(err){
		return false;
	}

	return true;
}

bool Controller::registerMethods(bool invalidating=false){
	vector<BPatch_function *> *fs;
	fs = analyser->getUsefulFunctions();

	BPatch_function *reg = analyser->getFunction("cine_method_registration");

	int mid = 0;
	vector<BPatch_snippet *> registrations;
	for(vector<BPatch_function *>::const_iterator mi = fs->begin();
			mi != fs->end(); mi++){
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

		if (invalidating){
			inst->timeFunctionInvalidating(f, mid);
		} else {
			inst->timeFunction(f, mid);
		}
		if(speedups != NULL){
			SpeedupMap::iterator record;
			for (record = speedups->begin(); record != speedups->end(); record++){
				if ((*record).first == f->getName()){
					registerSpeedup(mid, (*record).second);
				}
			}
		}

		mid++;
	}
	return true;
}

bool Controller::registerSpeedup(int mid, double speedup) {
	BPatch_function *reg = analyser->getFunction("cine_speedup_registration");
	BPatch_function * programStart = analyser->getFunction("_start");
	vector<BPatch_point *> *start = programStart->findPoint(BPatch_entry);
	vector<BPatch_snippet *> margs;
	BPatch_constExpr id(mid);
	margs.push_back(&id);
	ostringstream s;
	s << speedup;
	BPatch_constExpr speeduparg(s.str().c_str());
	margs.push_back(&speeduparg);
	BPatch_funcCallExpr speedupCall(*reg,margs);
	bool err;
	proc->oneTimeCode(speedupCall, &err);
	return err;
}

void Controller::getResults() {

	//change to be nicer
	BPatch_constExpr directory("/tmp");
	BPatch_function *print = analyser->getFunction("cine_exit_thread");

	vector<BPatch_snippet *>args;
	args.push_back(&directory);
	BPatch_funcCallExpr printCall(*print, args);

	bool err;
	this->proc->oneTimeCode(printCall, &err);

	if(err){
		cerr << "some onetime code error on exit " << endl;
	}
}

void Controller::registerSpeedups(SpeedupMap *speedups) {
	this->speedups = speedups;
}
