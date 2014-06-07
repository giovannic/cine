/*
 * DynamicLauncher.cpp
 *
 *  Created on: 20 May 2014
 *      Author: giovanni
 */

#include "DynamicLauncher.h"
#include "AsyncListener.h"
#include "debug.h"

DynamicLauncher::DynamicLauncher():Launcher() {
	bpatch->setDelayedParsing(false);
}

DynamicLauncher::~DynamicLauncher() {
	delete analyser;
	delete inst;
	delete ctrl;
}

BPatch_process *DynamicLauncher::createProcess(){
	vector<const char *>argv;
	for( std::vector<string>::const_iterator i = args->begin();
			i != args->end(); ++i){
	    argv.push_back((*i).c_str());
	}
	argv.push_back(NULL);

	char path[input->size()];
	strcpy(path, input->c_str());

	BPatch_process *proc = bpatch->processCreate(path, argv.data());
	app = proc;
	return proc;
}

void getResults(BPatch_thread *thread, BPatch_exitType e) {
	BPatch_constExpr directory("/tmp");
	BPatch_process *p = thread->getProcess();
	BPatch_function *print = Analyser(p->getImage()).getFunction("VEX::printResults");

	vector<BPatch_snippet *>args;
	args.push_back(&directory);
	BPatch_funcCallExpr printCall(*print, args);

	bool err;
	p->stopExecution();//this is silly
	p->oneTimeCode(printCall, &err);
	p->continueExecution();

	if(err){
		cerr << "some onetime code error on exit " << endl;
	}
}

void DynamicLauncher::listenResults(){
	bpatch->registerExitCallback(getResults);
}

bool DynamicLauncher::launch(){
	cout << "sleeping to avoid race condition in dyninst " << endl;
	sleep(5);
	app->continueExecution();
	while (!app->isTerminated())
		bpatch->waitForStatusChange();
	return true;
}

bool DynamicLauncher::setup(){
	this->app = createProcess();
	this->analyser = new Analyser(app->getImage());
	this->inst = new Instrumenter(analyser, app);
	this->ctrl = new Controller(inst, analyser, app);
	listener = new AsyncListener(bpatch);
//	inst->loadLibraries();
//	inst->timeFunction(analyser->getFunction("watch_count"), 0);
//	inst->timeFunction(analyser->getFunction("inc_count"), 0);
	app->beginInsertionSet();

	if(!inst->loadLibraries()){
		cerr << "libraries did not load" << endl;
		return false;
	}

	if(!ctrl->beginSimulator()){
		cerr << "vex simulator failed to start" << endl;
		return false;
	}

	if(!ctrl->registerMethods()){
		cerr << "unable to register methods" << endl;
		return false;
	}

	if(!inst->threadCreation()){
		cerr << "thread creation failed" << endl;
		return false;
	}

//	if(!inst->threadCreate()){
//		cerr << "thread creation failed" << endl;
//		return false;
//	}

//	ctrl->listenThreads();

	if(!inst->threadDestruction()){
		cerr << "thread destruction failed" << endl;
		return false;
	}

	if(!inst->instrumentContention()){
		cerr << "contention failed" << endl;
		return false;
	}

//	if(!listener->listen()){
//		cerr << "invalidation failed" << endl;
//		return false;
//	}

	//this causes a segfault as dyninst searches for lib=""
	ctrl->listenResults();
	inst->finalFunction("main");

	bool mod;
	app->finalizeInsertionSet(true, &mod);
	DEBUG_PRINT(("modified %d\n", mod));

	return true;
}
