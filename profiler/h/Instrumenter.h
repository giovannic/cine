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
    bool loadPthreads();
    bool timeFunction(BPatch_function *f, int methodId);
	bool timeFunctionInvalidating(BPatch_function *f, int methodId);
	bool timeFunctionCalls(BPatch_function *f, int methodId);
    bool instrumentThreadEntry(BPatch_thread *t);
    bool mainThreadCreation();
    bool instrumentThreadEntry(BPatch_process *p, BPatch_thread *t);
    bool instrumentThreadEntry(Dyninst::Address a);
    bool instrumentExit();
    bool finalFunction(string f);
    bool instrumentContention();
    bool threadCreation();
    bool threadDestruction();
    bool debugStart();
private:
    bool instrumentThreadEntry(BPatch_function *entryFunction);
    bool instrumentThreadEntry(BPatch_function *entryFunction,
    		BPatch_function *start, BPatch_function *end);
    bool threadStart();
    bool threadJoin();
    bool threadExit();
    bool threadMutex();
    bool instrumentSleep();
    bool wrapFunction(BPatch_function *f,
    		BPatch_function *newf, BPatch_function *oldf);
    bool wrapUp(vector<BPatch_point *> *exitPoints);
    Analyser *analyser;
	BPatch_addressSpace *app;
	vector<BPatchSnippetHandle *> *timers;
	bool replaceCalls(vector<BPatch_function *> &fs,
		BPatch_function *oldF, BPatch_function *newF);
	bool replaceCalls(vector<BPatch_function *> &fs,
		vector<BPatch_function *>&oldF, BPatch_function *newF);
	bool timeFunction(BPatch_function *f, int methodId,
		BPatch_function *timerStart, BPatch_function *timerStop);
};

#endif /* INSTRUMENTER_HPP_ */
