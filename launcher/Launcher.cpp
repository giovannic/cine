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

#include <sstream>
#include <string>
#include <fstream>
#include <regex>

using namespace std;
using namespace Dyninst;

Launcher::Launcher(){
	bpatch = new BPatch();
	bpatch->setTrampRecursive(true);
	speedups = new SpeedupMap();
}

Launcher::~Launcher(){
	delete bpatch;
	delete input;
	delete args;
	delete policy;
	delete mFile;
}

void Launcher::setInput(string &input) {
	this->input = new string(input);
}

bool Launcher::registerSpeedup(string& function, double speedup) {
	(*speedups)[analyser->getFunction(function)] = (long)speedup;
	return true;
}

bool Launcher::registerInvalidation(string& policy) {
	this->policy = new string(policy);
}

bool Launcher::registerMethodFile(string& mFile) {
	this->mFile = new string(mFile);
}

void Launcher::parseMethodFile() {
	if (mFile == NULL){
		return;
	}
	ifstream infile(mFile->c_str());
	string line;
	while (getline(infile, line))
	{
	    istringstream iss(line);
	    int id;
	    if(iss >> id){
	    	string token;
	    	stringstream name;
	    	while(iss >> token){
	    		if (token == "invalidated"){ //todo: regex
	    			cout << "invalidating " << name.str() << endl;
	    			analyser->ignore(analyser->getFunction(name.str()));
	    		} else {
	    			name << token;
	    		}
	    	}
	    }
//	    int n;
//	    string name, invalidated;
//	    if ((iss >> n >> name >> invalidated)) {
//		if(regex.)
//	    }
	}
	infile.close();
}

void Launcher::setArgs(vector<string> &args) {
	this->args = new vector<string>(args);
}
