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
#include "instrumentor.hpp"

//Dyninst
#include "BPatch.h"
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

const char *tocstring(const std::string & s)
{
   return s.c_str();
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

	BPatch_process *proc = bpatch.processCreate(path, argv.data());
	Instrumentor inst(proc->getImage());

	//Tell ProcControlAPI about our callback function
	Process::registerEventCallback(EventType::ThreadCreate, on_thread_create);

	//Run the process and wait for it to terminate.
	proc->continueExecution();
	while (!proc->isTerminated())
		Process::handleEvents(true);

	return true;
}
