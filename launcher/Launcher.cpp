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

Launcher::Launcher(string &input){
	this->input = new string(input);
	this->args = new vector<string>();
	this->args->push_back(input);
	bpatch = new BPatch();
}

Launcher::~Launcher(){
	delete input;
	delete args;
	delete bpatch;
}

void Launcher::add_arguments(string &args){
	//TODO parse arguments
	//there can only be one
	this->args->push_back(args);
}
