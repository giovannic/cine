/*
 * launcher.hpp
 *
 *  Created on: 5 Feb 2014
 *      Author: giovanni
 */

#ifndef LAUNCHER_HPP_
#define LAUNCHER_HPP_
#include "BPatch.h"
#include "Instrumenter.h"
#include "BPatch_function.h"
#include <map>

using namespace std;

class Launcher{
public:
	Launcher();
	virtual ~Launcher();
	virtual bool launch()=0;
	virtual bool setup()=0;
	virtual bool registerSpeedup(string &function, double speedup);
	virtual bool registerInvalidation(string &policy);
	virtual bool registerMethodFile(string &mFile);
	virtual void parseMethodFile();
	virtual void setInput(string &input);
	virtual void setArgs(vector<string> &args);
protected:
	Instrumenter *inst;
	Analyser *analyser;

	string *input;
	vector<string> *args;
	string *policy;
	string *mFile;
	SpeedupMap *speedups;

	BPatch *bpatch;
};

#endif /* LAUNCHER_HPP_ */
