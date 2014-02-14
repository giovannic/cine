/*
 * OProfile.cpp
 *
 *  Created on: 12 Feb 2014
 *      Author: giovanni
 */

#include "OProfile.hpp"
#include <stdlib.h>
#include <string>
#include <cstdio>
#include <iostream>
#include <signal.h>
#include <sstream>

using namespace std;

OProfile::OProfile(string *dir) {
	this->dir = dir;
	this->opctl = (*dir + "/operf");
	//"/home/giovanni/project/oprofile";
}

void OProfile::start(pid_t pid) {
	ostringstream args;
	args << "--pid " << pid;
	pid_t fk = fork();
	if (!fk) { /* in child */
		const char* copctl = opctl.c_str();
	    execl(copctl, copctl, "-p", pid, NULL);//"-t", "--session-dir", "/home/giovanni/project/whatif",  NULL);
	    exit(1);
	} else if (fk == -1) {
	    perror(args.str().c_str());
	}
	//parent code
	this->pid = fk;
}

void OProfile::stop() {
	kill(SIGINT, pid);
}

void OProfile::build_profile() {
	//run reporter

	cout << "building profile" << endl;
}

OProfile::~OProfile() {
	// TODO Auto-generated destructor stub
}

