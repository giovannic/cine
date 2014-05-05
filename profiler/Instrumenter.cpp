/*
 * functions.cpp
 *
 *  Created on: 8 Mar 2014
 *      Author: giovanni
 */

#include <vector>
#include "Instrumenter.h"
#include "Analyser.h"
#include "BPatch_function.h"
#include "BPatch_point.h"
#include "Symtab.h"
#include "Symbol.h"
using namespace std;
using namespace Dyninst::SymtabAPI;

Instrumenter::Instrumenter(Analyser *a, BPatch_addressSpace *as){
	analyser = a;
	app = as;
	//cache function calls
}


/*
 * until I can get find symbol to work pull this into analyser
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

bool Instrumenter::instrumentThreadEntry(Dyninst::Address a){
	BPatch_function *entry = analyser->findMethod(a);
	if (entry == NULL){
		cout << "no thread entry function found at " << a << endl;
		return false;
	}
	return instrumentThreadEntry(entry);

}

bool Instrumenter::instrumentThreadEntry(BPatch_thread *t){
	BPatch_function *entryFunction = t->getInitialFunc();
	return instrumentThreadEntry(entryFunction);

}

bool Instrumenter::instrumentThreadEntry(BPatch_process*p, BPatch_thread *t){
	bool err;

	BPatch_function *cineCreate = analyser->getFunction("cine_thread_create");
	vector<BPatch_snippet *>args;
	BPatch_funcCallExpr call(*cineCreate, args);

	p->oneTimeCode(call, &err);
	if (err){
		cerr << "one time code failing" << endl;
	}
	return false;
}
bool Instrumenter::instrumentThreadEntry(BPatch_function *entryFunction){
	vector<BPatch_point *> *entries = entryFunction->findPoint(BPatch_entry);
	vector<BPatch_point *> *exits = entryFunction->findPoint(BPatch_exit);
	if (entries->size() != 1){
		cerr << "no entry point found (or too many)" << endl;
	}
	if (entries->front()->getCurrentSnippets().size() > 0){
		//cannot time this method for the time being
		return true; //already instrumented
	}

	BPatch_function *cineCreate = analyser->getFunction("cine_start_thread");
	BPatch_function *cineDestroy = analyser->getFunction("cine_exit_thread");
	vector<BPatch_snippet *>args;
	BPatch_funcCallExpr entryCall(*cineCreate, args);
	BPatch_funcCallExpr exitCall(*cineDestroy, args);
	if(app->insertSnippet(entryCall, *entries) == NULL ||
			app->insertSnippet(exitCall, *entries) == NULL ){
		cerr << "entry instrumentation failed" << endl;
	}
	cout << "instrumented " << entryFunction->getName() << endl;

	return true;//this can fail
}

bool Instrumenter::insertThreadCalls(){

	BPatch_function *start = analyser->getFunction("start_thread");
	return instrumentThreadEntry(start);

//	BPatch_function *start = analyser->getFunction("start_thread");
//	BPatch_function *cineCreate= analyser->getFunction("cine_start_thread");
//	Module *symtab = convert(cineCreate->getModule());
//
//	Symbol *sym = findSymbol(symtab, "orig_start_thread");
//
//	if (sym == NULL){
//		cerr << "no replacement functions found" << endl;
//		return false;
//	}
//	app->wrapFunction(start, cineCreate, sym);
//	return true;

}

bool Instrumenter::timeFunction(BPatch_function *f, int methodId){
	vector<BPatch_point *> *entries = f->findPoint(BPatch_entry);
	vector<BPatch_point *> *exits = f->findPoint(BPatch_exit);
	BPatch_function *timerStart = analyser->getFunction("cine_timer_entry");
	BPatch_function *timerStop = analyser->getFunction("cine_timer_exit");
	vector<BPatch_snippet *>args;
	BPatch_constExpr mid(methodId);
	args.push_back(&mid);

	BPatch_funcCallExpr timerStartCall(*timerStart, args);
	BPatch_funcCallExpr timerStopCall(*timerStop, args);
	app->insertSnippet(timerStartCall, *entries);
	app->insertSnippet(timerStopCall, *exits);
	return true;
}

bool Instrumenter::beginSimulator(BPatch_process *p){
	//generate a call to VEX::initializeSimulator(NULL)
	BPatch_function * init = analyser->getFunction("VEX::initializeSimulator");
	vector<BPatch_snippet *> args;
	BPatch_nullExpr null;
	args.push_back(&null);
	BPatch_funcCallExpr initCall(*init, args);
	void * success = p->oneTimeCode(initCall);

	vector<BPatch_thread *>ts;
	p->getThreads(ts);

	//blocks for some reason
//	BPatch_function * initThread = analyser->getFunction("ThreadEventsBehaviour::onThreadMainStart");
//	BPatch_function * initThread = analyser->getFunction("cine_initial_thread");
//	vector<BPatch_snippet *> tArgs;

//	BPatch_threadIndexExpr tid;
//	tArgs.push_back(&tid);

//	BPatch_funcCallExpr initThreadCall(*initThread, tArgs);
//
//	ts.front()->oneTimeCode(initThreadCall);
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
