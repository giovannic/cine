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
	bpatch = new BPatch();
}

Launcher::~Launcher(){
	delete input;
	delete bpatch;
}
