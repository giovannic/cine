/*
 * launcher.cpp
 *
 *  Created on: 5 Feb 2014
 *      Author: giovanni
 */
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <cstring>

#include "Launcher.h"
#include "Instrumenter.h"
#include "Controller.h"

//Dyninst
#include "BPatch.h"

using namespace std;
using namespace Dyninst;


Launcher::Launcher(string &input){
	this->input = new string(input);
	this->args = new vector<string>();
	this->args->push_back(input);
	bpatch = new BPatch();
	setup();
}

Launcher::~Launcher(){
	BPatch_process *proc = dynamic_cast<BPatch_process *>(app);
	if (proc){
		proc->detach(false);
	}
	delete input;
	delete args;
	delete bpatch;
}

void Launcher::add_arguments(string &args){
	//TODO parse arguments
	//there can only be one
	this->args->push_back(args);
}

bool Launcher::launch(){
	BPatch_binaryEdit *bin = dynamic_cast<BPatch_binaryEdit *>(app);
	return bin->writeFile("vexbin");
}


BPatch_binaryEdit *Launcher::openBinary(){
	char path[input->size()];
	strcpy(path, input->c_str());
	return bpatch->openBinary(path);
}

BPatch_process *Launcher::createProcess(){
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

void Launcher::listenResults(){
	bpatch->registerExitCallback(getResults);
}

bool Launcher::setup(){

	bin = openBinary();

	analyser = new Analyser(bin->getImage());
	inst = new Instrumenter(analyser, bin->getImage()->getAddressSpace());

	if(!inst->loadLibraries()){
		cerr << "libraries did not load" << endl;
		return false;
	}

	/*
	if(!inst->beginSimulator(app)){
		cerr << "vex simulator failed to start" << endl;
		return false;
	}
	*/

	if(!inst->insertThreadCalls()){
		cerr << "could not patch pthread" << endl;
		return false;
	}

	return true;
}
