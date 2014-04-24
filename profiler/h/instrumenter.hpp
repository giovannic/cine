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

class Instrumenter {
public:
	Instrumenter(BPatch_image *img);
private:
	BPatch_addressSpace *app;
	BPatch_snippet start;
	BPatch_snippet stop;
	vector <BPatch_snippet*> args;
	void insertCall(BPatch_addressSpace *app, BPatch_function *input);
};

#endif /* INSTRUMENTER_HPP_ */
