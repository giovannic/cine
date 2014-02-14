/*
 * launcher.cpp
 *
 *  Created on: 5 Feb 2014
 *      Author: giovanni
 */
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "launcher.hpp"
#include <sys/wait.h>
#include <vector>

//Dyninst
#include "PCProcess.h"
#include "Event.h"

using namespace std;
using namespace Dyninst;
using namespace ProcControlAPI;

Process::cb_ret_t on_thread_create(Event::const_ptr ev) {
	cout << "thread created" << endl;
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

	Process::ptr proc = Process::createProcess(*input, *args);

	//start profiler
	if(profiler != NULL){
		profiler->start(proc.get()->getPid());
	}

	//Tell ProcControlAPI about our callback function
	Process::registerEventCallback(EventType::ThreadCreate, on_thread_create);
	//Run the process and wait for it to terminate.
	proc->continueProc();
	while (!proc->isTerminated())
		Process::handleEvents(true);

	if(profiler != NULL){
		profiler->stop();
	}

	return true;
}
