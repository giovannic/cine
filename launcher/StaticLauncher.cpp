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

#include "StaticLauncher.h"
#include "Instrumenter.h"
#include "Controller.h"

//Dyninst
#include "BPatch.h"

using namespace std;
using namespace Dyninst;

StaticLauncher::StaticLauncher():Launcher() {

}

StaticLauncher::~StaticLauncher() {
}

bool StaticLauncher::launch(){
	return bin->writeFile("vexbin");
}

BPatch_binaryEdit *StaticLauncher::openBinary(){
	char path[input->size()];
	strcpy(path, input->c_str());
	return bpatch->openBinary(path);
}

bool StaticLauncher::setup(){
	this->bin = openBinary();
	this->analyser = new Analyser(bin->getImage());
	this->inst = new Instrumenter(analyser, bin);

//	inst->loadLibraries();
//	inst->timeFunction(analyser->getFunction("inc_count"), 0);

	if(!inst->loadLibraries()){
		cerr << "libraries did not load" << endl;
		return false;
	}

	if(!inst->loadPthreads()){
		cerr << "libraries did not load" << endl;
		return false;
	}

	if(!inst->initCalls()){
		cerr << "init calls failed" << endl;
		return false;
	}

	if(!inst->threadCreation()){
		cerr << "thread creation failed" << endl;
		return false;
	}
	if(!inst->threadDestruction()){
		cerr << "thread destruction failed" << endl;
		return false;
	}
	if(!inst->instrumentContention()){
		cerr << "contention failed" << endl;
		return false;
	}

//	if(!inst->insertThreadCalls()){
//		cerr << "thread calls failed" << endl;
//		return false;
//	}

	if(!inst->instrumentExit()){
		inst->finalFunction("main");
		//if QoS method specified make the endpoint at the exit
		//may need better logic here
	}

	return true;
}
