/*
 * functions.cpp
 *
 *  Created on: 8 Mar 2014
 *      Author: giovanni
 */

#include <vector>
#include "Instrumenter.h"
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

/*
 * until I can get find symbol to work
 */
Symbol *findSymbol(Module *m, string s){
	vector <Symbol *> syms;
	//get the method names
	m->getAllSymbols(syms);
	for( std::vector<Symbol *>::const_iterator i = syms.begin();
			i != syms.end(); ++i){
		Symbol *f = *i;
		if (f->getPrettyName() == s){
			return f;
		}
	}
	return NULL;
//	symtab->findSymbol(matches, "orig_thread_create\0",
//			Symbol::ST_FUNCTION,
//			prettyName,
//			false,
//			false,
//			true);

}

bool Instrumenter::insertThreadCalls(){
	BPatch_function *create = getFunction("pthread_create");
	BPatch_function *cineCreate= getFunction("cine_thread_create");
	BPatch_function *replaceCreate = getFunction("orig_thread_create");
	Module *symtab = convert(cineCreate->getModule());

	Symbol *sym = findSymbol(symtab, "orig_thread_create");

	if (sym == NULL){
		cerr << "no replacement functions found" << endl;
		return false;
	}
	app->wrapFunction(create, cineCreate, sym);
	return true;
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

	if(!app->loadLibrary("libvex.so")){
		cerr << "target loading of vex failed" << endl;
		return false;
	}

	if(!app->loadLibrary("libcine.so")){
		cerr << "target loading of cine failed" << endl;
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

