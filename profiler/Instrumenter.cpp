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
	timers = new vector<BPatchSnippetHandle *>();
	//cache function calls
}

Instrumenter::~Instrumenter(){
	delete timers;
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

bool Instrumenter::instrumentMain(){

	BPatch_function *entryFunction = analyser->getFunction("_start");

	BPatch_function *cineCreate = analyser->getFunction("cine_initial_thread");
	BPatch_function *cineDestroy = analyser->getFunction("cine_exit_thread");

	cout << entryFunction->findPoint(BPatch_exit)->front()->getAddress() << endl;
	vector<BPatch_point *> *entries = entryFunction->findPoint(BPatch_entry);

	vector<BPatch_snippet *>args;
	BPatch_funcCallExpr entryCall(*cineCreate, args);

	BPatchSnippetHandle *entrySnippet = app->insertSnippet(entryCall, *entries);

	if(entrySnippet == NULL){
		cerr << "entry instrumentation failed" << endl;
	} else {
		timers->push_back(entrySnippet);
	}

	cout << "instrumented " << entryFunction->getName() << endl;

//	return instrumentThreadEntry(entryFunction, cineCreate, cineDestroy);

//	vector<BPatch_function *> ms = analyser->getAllFunctions("main");
//	BPatch_function *cineCreate = analyser->getFunction("cine_initial_thread");
//	BPatch_function *cineDestroy = analyser->getFunction("cine_exit_thread");
//
//	bool success = true;
//	for (vector<BPatch_function *>::const_iterator mi = ms.begin();
//			mi != ms.end(); mi++){
//		BPatch_function *m = *mi;
//		cout << m->findPoint(BPatch_entry)->front()->getAddress() << endl;
//		success = success && instrumentThreadEntry(m, cineCreate, cineDestroy);
//	}
//
//	return success;

}


bool Instrumenter::instrumentThreadEntry(BPatch_function *entryFunction){
	BPatch_function *cineCreate = analyser->getFunction("cine_start_thread");
	BPatch_function *cineDestroy = analyser->getFunction("cine_exit_thread");
	return instrumentThreadEntry(entryFunction, cineCreate, cineDestroy);
}

//may not work - join, exit
bool Instrumenter::instrumentThreadEntry(BPatch_function *entryFunction,
		BPatch_function *start, BPatch_function *end){
	vector<BPatch_point *> *entries = entryFunction->findPoint(BPatch_entry);
	vector<BPatch_point *> *exits = entryFunction->findPoint(BPatch_exit);
	if (entries->size() != 1){
		cerr << "no entry point found (or too many)" << endl;
	}

//	vector<BPatchSnippetHandle *> snippets = entries->front()->getCurrentSnippets();
//	if (snippets.empty()){ //HACK: timing snippet
//		cout << snippets.size() << " snippets" << endl;
//		cout << snippets.front()->getFunc()->getName() << endl;
//		return true;
//	}


//	for (vector<BPatchSnippetHandle *>::const_iterator si = timers->begin();
//			si != timers->end(); si++){
//		BPatchSnippetHandle *s = *si;
//		cout << s->getFunc()->getName() << endl;
//		if(s->getFunc()->getName() == entryFunction->getName()){
//			return true;
//		}
//	}


	vector<BPatch_snippet *>args;
	BPatch_funcCallExpr entryCall(*start, args);
	BPatch_funcCallExpr exitCall(*end, args);

	BPatchSnippetHandle *entrySnippet = app->insertSnippet(entryCall, *entries);

	if(entrySnippet == NULL ||
			app->insertSnippet(exitCall, *entries) == NULL){
		cerr << "entry instrumentation failed" << endl;
	} else {
		timers->push_back(entrySnippet);
	}

	cout << "instrumented " << entryFunction->getName() << endl;

	return true;//this can fail
}

bool Instrumenter::insertThreadCalls(){

	//doesn't work
	BPatch_function *start = analyser->getFunction("start_thread");
	vector<BPatch_point *> *exits = start->findPoint(BPatch_exit);
	BPatch_function *cineCreate = analyser->getFunction("cine_start_thread");
	vector<BPatch_snippet *>args;
	BPatch_funcCallExpr createCall(*cineCreate, args);
	if(app->insertSnippet(createCall, *exits) != NULL){
		return true;
	}
	return false;
//	return instrumentThreadEntry(start);

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

	if(!app->loadLibrary("libpthread.so.0")){
		cerr << "target loading of pthread failed" << endl;
		return false;
	}
	return true;

}
