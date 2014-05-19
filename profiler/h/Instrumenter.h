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

#include "Analyser.h"

using namespace std;

class Instrumenter {
public:
	Instrumenter(Analyser *a, BPatch_addressSpace *as);
	~Instrumenter();
	bool initCalls();
    bool insertThreadCalls();
    bool loadLibraries();
    bool timeFunction(BPatch_function *f, int methodId);
    bool instrumentThreadEntry(BPatch_thread *t);
    bool trackMainThread();
    bool instrumentThreadEntry(BPatch_process *p, BPatch_thread *t);
    bool instrumentThreadEntry(Dyninst::Address a);
    bool instrumentExit();
private:
    bool instrumentThreadEntry(BPatch_function *entryFunction);
    bool instrumentThreadEntry(BPatch_function *entryFunction,
    		BPatch_function *start, BPatch_function *end);
    bool threadStart();
    bool threadCreation();
    bool threadJoin();
    bool threadExit();
    Analyser *analyser;
	BPatch_addressSpace *app;
	vector<BPatchSnippetHandle *> *timers;
};

#endif /* INSTRUMENTER_HPP_ */
