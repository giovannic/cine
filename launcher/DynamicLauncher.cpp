/*
 * DynamicLauncher.cpp
 *
 *  Created on: 20 May 2014
 *      Author: giovanni
 */

#include "DynamicLauncher.h"

DynamicLauncher::DynamicLauncher(string &input):Launcher(input) {
}

DynamicLauncher::~DynamicLauncher() {
}

BPatch_process *DynamicLauncher::createProcess(){
	vector<const char *>argv;
	for( std::vector<string>::const_iterator i = args->begin();
			i != args->end(); ++i){
	    argv.push_back(i->c_str());
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
