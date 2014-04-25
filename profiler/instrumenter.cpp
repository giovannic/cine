/*
 * functions.cpp
 *
 *  Created on: 8 Mar 2014
 *      Author: giovanni
 */

#include <vector>
#include "instrumenter.hpp"
#include "BPatch_function.h"
#include "BPatch_point.h"
#include "Symtab.h"
#include "Symbol.h"
using namespace std;
using namespace Dyninst::SymtabAPI;

Instrumenter::Instrumenter(BPatch_image *input){

	img = input;
	app = img->getAddressSpace();

}

BPatchSnippetHandle *Instrumenter::insertCall(BPatch_function &input,
		BPatch_function &libFunc, vector<BPatch_snippet*> &args){

	BPatch_funcCallExpr lfCall(libFunc, args);
	return NULL; // not finished //app->insertSnippet(lfCall);
}

bool Instrumenter::insertThreadCalls(){
	BPatch_function *create = getFunction("pthread_create");
	BPatch_function *cineCreate= getFunction("cine_thread_create");
	Module *symtab = convert(cineCreate->getModule());
	vector<Symbol *> matches;
	symtab->findSymbol(matches, "orig_thread_create",
			Symbol::ST_UNKNOWN,
			mangledName,
			false,
			false,
			true);
	if (matches.size() != 1){
		cerr << matches.size() << " replacement functions found" << endl;
		return false;
	}
	app->wrapFunction(create, cineCreate, matches.front());
	return true;
/*
	vector<BPatch_snippet*>args;
	BPatch_funcCallExpr lfCall(*cineCreate, args);

	//TODO: find the right point
	return app->insertSnippet(lfCall,
			*create->findPoint(BPatch_locEntry),
			BPatch_callBefore);
			*/
}

bool Instrumenter::beginSimulator(BPatch_process *p){
	//generate a call to VEX::initializeSimulator(NULL)
	BPatch_function * init = getFunction("VEX::initializeSimulator");
	vector<BPatch_snippet *> args;
	BPatch_nullExpr null;
	args.push_back(&null);
	BPatch_funcCallExpr initCall(*init, args);
	void * success = p->oneTimeCode(initCall);
	return (bool)(long) success;
}

bool Instrumenter::loadLibraries(){

	if(!app->loadLibrary("libcine.so")){
		cerr << "target loading of cine failed" << endl;
		return false;
	}

	if(!app->loadLibrary("libvex.so")){
		cerr << "target loading of vex failed" << endl;
		return false;
	}

	return true;

}

BPatch_function *Instrumenter::getFunction(string s){

	vector<BPatch_function *> fs;
	const char *sArg = s.c_str();
	img->findFunction(sArg, fs);
	if (fs.size() != 1){
		cerr << "[warning] " << fs.size() << " x " << s << endl;
		return NULL;
	}
	return fs.front();

}

BPatch_function *Instrumenter::getPCreate(){
	return getFunction("pthread_create");
}
