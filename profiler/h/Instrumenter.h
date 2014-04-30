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
	bool beginSimulator(BPatch_process *p);
    bool insertThreadCalls();
    bool loadLibraries();
    bool timeFunction(BPatch_function *f, int methodId);
private:
    Analyser *analyser;
	BPatch_addressSpace *app;
};

#endif /* INSTRUMENTER_HPP_ */
