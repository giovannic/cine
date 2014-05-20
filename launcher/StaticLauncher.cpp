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

StaticLauncher::StaticLauncher(string &input):Launcher(input) {
	this->bin = openBinary();
	this->analyser = new Analyser(bin->getImage());
	this->inst = new Instrumenter(analyser, bin);
	setup();
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
//	inst->loadLibraries();
//	inst->timeFunction(analyser->getFunction("inc_count"), 0);

	if(!inst->loadLibraries()){
		cerr << "libraries did not load" << endl;
		return false;
	}


	if(!inst->initCalls()){
		cerr << "init calls failed" << endl;
		return false;
	}

	if(!inst->insertThreadCalls()){
		cerr << "thread calls failed" << endl;
		return false;
	}

	if(!inst->instrumentExit()){
		cout << "no explicit exit point falling back on thread counting" << endl;
		//if QoS method specified make the endpoint at the exit
		//may need better logic here
		inst->finalFunction("main");
	}

	return true;
}