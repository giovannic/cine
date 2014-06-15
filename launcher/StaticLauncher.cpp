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
#include "debug.h"

using namespace std;
using namespace Dyninst;

StaticLauncher::StaticLauncher():Launcher() {
	bpatch->setInstrStackFrames(true); //TODO: remove when understood
}

StaticLauncher::~StaticLauncher() {
}

bool StaticLauncher::launch(){
	return bin->writeFile("vexbin");
}

BPatch_binaryEdit *StaticLauncher::openBinary(){
	char path[input->size()];
	strcpy(path, input->c_str());
	return bpatch->openBinary(path, true);
}

bool StaticLauncher::setup(){
	this->bin = openBinary();
	this->analyser = new Analyser(bin->getImage());
	this->inst = new Instrumenter(analyser, bin);

//	inst->loadLibraries();
//	inst->timeFunction(analyser->getFunction("inc_count"), 0);
	bin->beginInsertionSet();

	if(!inst->loadLibraries()){
		cerr << "libraries did not load" << endl;
		return false;
	}

//	if(!inst->loadPthreads()){
//		cerr << "pthreads did not reload" << endl;
//		return false;
//	}

	if(!inst->staticInitCalls()){
		cerr << "init calls failed" << endl;
		return false;
	}

	if(mFile != NULL){
		parseMethodFile();
	}

	if(!inst->registerMethods(*speedups)){
		return false;
	}

	if(policy != NULL){
		if(!inst->addPolicy(*policy)){
			cerr << "unable to register invalidation" << endl;
			return false;
		}
	}

	if(!inst->relocatePthreads()){
		cerr << "relocation failed" << endl;
		return false;
	}


//	if(!inst->threadCreation()){
//		cerr << "thread creation failed" << endl;
//		return false;
//	}
//
//	if(!inst->threadDestruction()){
//		cerr << "thread destruction failed" << endl;
//		return false;
//	}
//
//	if(!inst->instrumentContention()){
//		cerr << "contention failed" << endl;
//		return false;
//	}
//
////	if(!inst->time()){
////		cerr << "time failed" << endl;
////		return false;
////	}
//
//	if(!inst->instrumentExit()){
//		inst->finalFunction("main");
//		//if QoS method specified make the endpoint at the exit
//		//may need better logic here
//	}

	bool mod;
	bin->finalizeInsertionSet(true, &mod);
	DEBUG_PRINT(("modified %d\n", mod));

	return true;
}
