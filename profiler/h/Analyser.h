/*
 * Analyser.h
 *
 *  Created on: 29 Apr 2014
 *      Author: giovanni
 */

#ifndef ANALYSER_H_
#define ANALYSER_H_

#include "BPatch_function.h"

class Analyser {
public:
	Analyser(BPatch_image *img);
	virtual ~Analyser();
	bool getUsefulFunctions(vector <BPatch_function *> &fs);
	bool getUsefulModules(vector<BPatch_module *> &ms);
	BPatch_function *getFunction(string s);
	BPatch_function *getStartThread();
	vector<BPatch_function *>getAllFunctions(string s);
    BPatch_function *findMethod(Dyninst::Address a);
    BPatch_point *hasCall(BPatch_function *f, string calleeName);
    void getCalls(BPatch_function *f, BPatch_function *newF, vector<BPatch_point *> &pts);
private:
	BPatch_image *img;
};

#endif /* ANALYSER_H_ */
