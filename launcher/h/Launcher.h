/*
 * launcher.hpp
 *
 *  Created on: 5 Feb 2014
 *      Author: giovanni
 */

#ifndef LAUNCHER_HPP_
#define LAUNCHER_HPP_
#include "BPatch.h"
#include "Instrumenter.h"

using namespace std;

class Launcher{
public:
	Launcher(string &input);
	virtual ~Launcher();
	virtual bool launch()=0;
	void add_arguments(string &args);
protected:
	Instrumenter *inst;
	Analyser *analyser;

	string *input;
	vector<string> *args;

	BPatch *bpatch;
};

#endif /* LAUNCHER_HPP_ */
