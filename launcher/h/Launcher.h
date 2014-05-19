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

//needs to be refactored to a static and dynamic loader
class Launcher{
public:

	Launcher(string &input);
	virtual ~Launcher();
	bool launch();
	void add_arguments(string &args);
private:
	bool setup();
	void listenResults();
	BPatch_process *createProcess();
	BPatch_binaryEdit *openBinary();
	string *input;
	vector<string> *args;

	Instrumenter *inst;
	Analyser *analyser;

	BPatch *bpatch;
	BPatch_addressSpace *app;
	BPatch_binaryEdit *bin;
};

#endif /* LAUNCHER_HPP_ */
