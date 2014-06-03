/*
 * Analyser.cpp
 *
 *  Created on: 29 Apr 2014
 *      Author: giovanni
 */

#include "Analyser.h"
#include <vector>
#include "BPatch_function.h"
#include "BPatch_point.h"
#include "BPatch_image.h"
#include "debug.h"
#include <iostream>

using namespace std;

bool Analyser::getUsefulModules(vector<BPatch_module *> &ms){
	vector<BPatch_module *>allms;

	//back inserter alternative?
	img->getModules(ms);
	for (vector <BPatch_module *>::iterator it = ms.begin();
			it != ms.end(); it++){
		BPatch_module *m = *it;

		if(!m->isSharedLib()){
			ms.push_back(m);
		}
	}

	return true;
}

bool Analyser::getUsefulFunctions(vector<BPatch_function *> &fs){
	vector<BPatch_module *>ms;
	vector<BPatch_function *> *mfs;

	//back inserter alternative?
	img->getModules(ms);
	for (vector <BPatch_module *>::iterator it = ms.begin();
			it != ms.end(); it++){
		BPatch_module *m = *it;

		if(!m->isSharedLib() && !m->isSystemLib()){
			mfs = m->getProcedures();
			fs.insert(fs.end(), mfs->begin(), mfs->end());
		}
	}

	return true;
}


vector<BPatch_function *>Analyser::getAllFunctions(string s){

	vector<BPatch_function *> fs;
	const char *sArg = s.c_str();
	img->findFunction(sArg, fs);
	return fs;

}

BPatch_function *Analyser::getStartThread(){
	vector<BPatch_module *> *ms = img->getModules();
	for (vector<BPatch_module *>::const_iterator mi = ms->begin();
			mi != ms->end(); mi++){
		char n[50];
		(*mi)->getName(n, 50);
	}
	return NULL;
}

BPatch_function *Analyser::getFunction(string s){

	vector<BPatch_function *> fs;
	const char *sArg = s.c_str();
	img->findFunction(sArg, fs);
	if (fs.size() != 1){
		DEBUG_PRINT(("[warning] %d x %s\n", fs.size(), s.c_str()));
		if (fs.size() == 0){
			return NULL;
		}
	}
	return fs.front();

}

Analyser::Analyser(BPatch_image *img) {
	this->img = img;
}

Analyser::~Analyser() {
}

BPatch_function* Analyser::findMethod(Dyninst::Address a) {
	return img->findFunction(a);
}

//TODO:fix this for static analysis
void Analyser::getCalls(BPatch_function *f, BPatch_function *newF, vector<BPatch_point *> &pts){
	vector<BPatch_point *>calls;
	f->getCallPoints(calls);
	bool has = false;

//	cout << "calls found: " << calls.size() << endl;
	for(vector<BPatch_point *>::const_iterator c = calls.begin();
			c != calls.end(); c++){
		BPatch_point *cp = *c;
		BPatch_function *callee = cp->getCalledFunction();
		if(callee != NULL && (callee->getDemangledName() == newF->getDemangledName())){
			pts.push_back(cp);
		}
	}
}

//TODO:fix this for static analysis
BPatch_point *Analyser::hasCall(BPatch_function *f, BPatch_function *calleeF){
	vector<BPatch_point *>calls;
	f->getCallPoints(calls);

	for(vector<BPatch_point *>::const_iterator c = calls.begin();
			c != calls.end(); c++){
		BPatch_point *cp = *c;
		if(cp == NULL){
			//this is broken for static execution
			return NULL;
		}
		BPatch_function *callee = cp->getCalledFunction();
		if(callee != NULL && (callee->getDemangledName() == calleeF->getDemangledName())){
			return cp;
		}
	}
	return NULL;
}

//	BPatch_function *mutexLock = analyser->getFunction("pthread_mutex_lock");
//	BPatch_function *cineMutexLock = analyser->getFunction("cine_mutex_lock");
//
//	vector<BPatch_point *> callers;
//	mutexLock->getCallerPoints(callers);
//
//	cout << callers.size() << " callers of pthread_mutex_lock" << endl;
//	for(vector<BPatch_point *>::const_iterator it = callers.begin();
//			it != callers.end(); it++){
//		BPatch_point *p = *it;
//		cout << p->getFunction()->getName() << " calls pthread_mutex_lock" << endl;
//		if(!p->getFunction()->getModule()->isSharedLib()){
//			if(!app->replaceFunctionCall(*p, *cineMutexLock)){
//				cout << "failed to replace function call" << endl;
//			}
//		}
//	}
