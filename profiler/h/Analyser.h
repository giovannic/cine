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
	BPatch_function *getFunction(string s);
private:
	BPatch_image *img;
};

#endif /* ANALYSER_H_ */