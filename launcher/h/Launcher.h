/*
 * launcher.hpp
 *
 *  Created on: 5 Feb 2014
 *      Author: giovanni
 */

#ifndef LAUNCHER_HPP_
#define LAUNCHER_HPP_
#include <vector>
#include "BPatch.h"
#include "Instrumenter.h"

using namespace std;

class Launcher{
public:

	Launcher(string *input);
	void listenResults();
	virtual ~Launcher();
	bool setup();
	void launch();
	void add_arguments(string *args);
	BPatch_process *createProcess();
private:
	string *input;
	vector<string> *args;

	Instrumenter *inst;
	Analyser *analyser;

	BPatch *bpatch;
	BPatch_process *app;
};

#endif /* LAUNCHER_HPP_ */
