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


Launcher::Launcher(string *input){
	this->input = new string(*input);
	this->args = new vector<string>();
	this->args->push_back(*input);
	bpatch = new BPatch();
}

Launcher::~Launcher(){
	app->terminateExecution();
	delete input;
	delete args;
	delete bpatch;
}

void Launcher::add_arguments(string *args){
	//TODO parse arguments
	//there can only be one
	this->args->push_back(*args);
}

void Launcher::launch(){
	app->continueExecution();
	while (!app->isTerminated())
		bpatch->waitForStatusChange();
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

	app = bpatch->processCreate(path, argv.data());
	return app;
}

bool Launcher::setup(){

	app = createProcess();

	inst = new Instrumenter(app->getImage());
	Controller *controller = new Controller();

	if(!inst->loadLibraries()){
		cerr << "libraries did not load" << endl;
		return false;
	}

	if(!inst->beginSimulator(app)){
		cerr << "vex simulator failed to start" << endl;
		return false;
	}

	if(!inst->insertThreadCalls()){
		cerr << "could not patch pthread" << endl;
		return false;
	}

	return true;
}
