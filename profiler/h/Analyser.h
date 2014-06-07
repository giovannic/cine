/*
 * Analyser.h
 *
 *  Created on: 29 Apr 2014
 *      Author: giovanni
 */

#ifndef ANALYSER_H_
#define ANALYSER_H_

#include "BPatch_function.h"
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <string>

using namespace std;
typedef unordered_map<string, BPatch_function*> SearchCache_t;
typedef pair<string, BPatch_function*> SearchRecord_t;
class Analyser {
public:
	Analyser(BPatch_image *img);
	virtual ~Analyser();
	vector<BPatch_function *> *getUsefulFunctions();
	bool getUsefulModules(vector<BPatch_module *> &ms);
	BPatch_function *getFunction(string s);
	BPatch_function *getStartThread();
	vector<BPatch_function *>getAllFunctions(string s);
    BPatch_function *findMethod(Dyninst::Address a);
    BPatch_point *hasCall(BPatch_function *f, BPatch_function *calleeF);
    void getCalls(BPatch_function *f, BPatch_function *newF, vector<BPatch_point *> &pts);
private:
    bool callMatches(BPatch_function *a, BPatch_function *b);
	BPatch_image *img;
	SearchCache_t *searchCache;
	vector<BPatch_function*>*functionSet;
    void populateCache();
};

#endif /* ANALYSER_H_ */
