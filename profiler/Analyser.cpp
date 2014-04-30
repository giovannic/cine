/*
 * Analyser.cpp
 *
 *  Created on: 29 Apr 2014
 *      Author: giovanni
 */

#include "Analyser.h"
#include <vector>
#include "BPatch_function.h"
#include "BPatch_image.h"
#include <iostream>

using namespace std;

bool Analyser::getUsefulFunctions(vector<BPatch_function *> &fs){
	vector<BPatch_module *>ms;
	vector<BPatch_function *> *mfs;

	//back inserter alternative?
	img->getModules(ms);
	for (vector <BPatch_module *>::iterator it = ms.begin();
			it != ms.end(); it++){
		BPatch_module *m = *it;

		if(!m->isSharedLib()){
			mfs = m->getProcedures();
			fs.insert(fs.end(), mfs->begin(), mfs->end());
		}
	}

	return true;
}

BPatch_function *Analyser::getFunction(string s){

	vector<BPatch_function *> fs;
	const char *sArg = s.c_str();
	img->findFunction(sArg, fs);
	if (fs.size() != 1){
		cerr << "[warning] " << fs.size() << " x " << s << endl;
		return NULL;
	}
	return fs.front();

}

Analyser::Analyser(BPatch_image *img) {
	this->img = img;
}

Analyser::~Analyser() {
}

