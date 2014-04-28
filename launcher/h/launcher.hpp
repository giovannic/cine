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
#include "instrumenter.hpp"

using namespace std;

class Launcher{
public:
	string *input = NULL;
	vector<string> *args = NULL;
	Instrumenter *inst = NULL;
	BPatch *bpatch = NULL;

	Launcher(string *input);
	virtual ~Launcher();
	bool setup();
	void launch();
	void add_arguments(string *args);
	BPatch_process *createProcess();
private:
	BPatch_process *app = NULL;
};


#endif /* LAUNCHER_HPP_ */
