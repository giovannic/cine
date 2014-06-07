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
#include "Analyser.h"
#include "Controller.h"

//Dyninst
#include "BPatch.h"

using namespace std;
using namespace Dyninst;

Launcher::Launcher(){
	bpatch = new BPatch();
}

Launcher::~Launcher(){
	delete bpatch;
	delete input;
	delete args;
}

void Launcher::setInput(string &input) {
	this->input = new string(input);
}

bool Launcher::registerSpeedup(string& function, double speedup) {
	cout << "speedup not yet implemented" << endl;
	return false;
}

void Launcher::setArgs(vector<string> &args) {
	this->args = new vector<string>(args);
}
