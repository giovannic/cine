/*
 * launcher.cpp
 *
 *  Created on: 5 Feb 2014
 *      Author: giovanni
 */
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <vector>
#include <cstring>

#include "launcher.hpp"
#include "instrumenter.hpp"

//Dyninst
#include "BPatch.h"
#include "PCProcess.h"
#include "Event.h"

//vex
#include "VTF.h"

using namespace std;
using namespace Dyninst;
using namespace ProcControlAPI;
using namespace VEX;


//TODO:make this execute on the target with irpc or callback delivery
Process::cb_ret_t on_thread_create(Event::const_ptr ev) {
	Thread::const_ptr t = ev->getThread();
	long id = t->getTID();

	if (t->isInitialThread()){
		threadEventsBehaviour->onThreadMainStart(id);
	} else {
		threadEventsBehaviour->onStart(id, "subthread");
	}
	return Process::cbDefault;
}

Process::cb_ret_t on_thread_exit(Event::const_ptr ev) {
	Thread::const_ptr t = ev->getThread();
	long id = t->getTID();

	threadEventsBehaviour->onJoin(id);
	return Process::cbDefault;
}

Launcher::Launcher(string *input){
	this->input = input;
	this->args = new vector<string>();
	this->args->push_back(*input);
	this->profiler = NULL;
}

void Launcher::add_arguments(string *args){
	//TODO parse arguments
	this->args->push_back(*args);
}

bool Launcher::launch(){

	BPatch bpatch;

	vector<const char *>argv;
	for( std::vector<string>::const_iterator i = args->begin(); i != args->end(); ++i){
	    argv.push_back(i->c_str());
	}
	argv.push_back(NULL);

	char path[input->size()];
	strcpy(path, input->c_str());

	bool success = initializeSimulator(NULL);
	if(!success){
		cerr << "vex faliure" << endl;
		return false;
	}

	BPatch_process *app = bpatch.processCreate(path, argv.data());
	Instrumenter inst(app->getImage());

	//Tell ProcControlAPI about our callback function
	Process::registerEventCallback(EventType::UserThreadCreate, on_thread_create);
	Process::registerEventCallback(EventType::UserThreadDestroy, on_thread_exit);
	//Run the process and wait for it to terminate.
	app->continueExecution();
	while (!app->isTerminated())
		bpatch.waitForStatusChange();

	return true;
}
