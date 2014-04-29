/*
 * instrumentor.hpp
 *
 *  Created on: 8 Mar 2014
 *      Author: giovanni
 */

#ifndef INSTRUMENTER_HPP_
#define INSTRUMENTER_HPP_

#include "BPatch.h"
#include "BPatch_image.h"
#include "BPatch_point.h"
#include "BPatch_function.h"

using namespace std;

//privatise and remove
class Instrumenter {
public:
	Instrumenter(BPatch_image *img);
	BPatch_function *getPCreate();
	BPatch_function *getFunction(string s);
	BPatch_image *img;
	BPatch_addressSpace *app;
	BPatch_snippet start;
	BPatch_snippet stop;
	vector <BPatch_snippet*> args;
	BPatchSnippetHandle *insertCall(
			BPatch_function &input,
			BPatch_function &libfunc,
			vector<BPatch_snippet *> &args);
	bool beginSimulator(BPatch_process *p);
    bool insertThreadCalls();
    bool loadLibraries();
};

#endif /* INSTRUMENTER_HPP_ */
