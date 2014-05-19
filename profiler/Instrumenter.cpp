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
#include "BPatch_flowGraph.h"
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

bool Instrumenter::trackMainThread(){

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


	//check that we're not already tracking
	for (vector<BPatchSnippetHandle *>::const_iterator si = timers->begin();
			si != timers->end(); si++){
		BPatchSnippetHandle *s = *si;
		if (s->getFunc()->getName() == entryFunction->getName()){
			return true;
		}
	}

	vector<BPatch_snippet *>args;
	BPatch_funcCallExpr entryCall(*start, args);
	BPatch_funcCallExpr exitCall(*end, args);

	BPatchSnippetHandle *entrySnippet = app->insertSnippet(entryCall, *entries, BPatch_firstSnippet);

	//make sure that this is the last call that is made
	if(entrySnippet == NULL ||
			app->insertSnippet(exitCall, *exits, BPatch_lastSnippet) == NULL){
		cerr << "entry instrumentation failed" << endl;
	} else {
		timers->push_back(entrySnippet);
	}

	cout << "instrumented " << entryFunction->getName() << endl;

	return true;//this can fail
}
bool Instrumenter::threadCreation(){
	BPatch_function *create = analyser->getFunction("pthread_create");

//	BPatch_function *beforeCreate = analyser->getFunction("cine_before_create");
//	BPatch_paramExpr thread(0);
//	vector<BPatch_snippet *> args;
//	args.push_back(&thread);
//	BPatch_funcCallExpr beforeCall(*beforeCreate, args);
//
//	vector<BPatch_point*> *entries = create->findPoint(BPatch_exit);
//	return(app->insertSnippet(beforeCall, *entries) != NULL);

	BPatch_function *cineCreate = analyser->getFunction("cine_thread_create");
	BPatch_function *replaceCreate = analyser->getFunction("orig_thread_create");
	Module *symtab = convert(cineCreate->getModule());

	Symbol *sym = findSymbol(symtab, "orig_thread_create");

	if (sym == NULL){
		cerr << "no replacement functions found" << endl;
		return false;
	}

	if (!app->wrapFunction(create, cineCreate, sym)){
		return false;
	}
	return true;
}

bool Instrumenter::threadStart(){
	BPatch_function *start = analyser->getFunction("start_thread");
	vector<BPatch_point *> possible;
	start->getCallPoints(possible);

	for (vector<BPatch_point *>::const_iterator pi = possible.begin();
			pi != possible.end(); pi++){
		BPatch_point *callSite = *pi;
		if(callSite->isDynamic()){
			//this is as good as guess as any
			BPatch_function *cineStart = analyser->getFunction("cine_start_thread");
			vector<BPatch_snippet *> args;
			BPatch_funcCallExpr startCall(*cineStart, args);
			if(app->insertSnippet(startCall, *callSite) == NULL){
				return false;
			}
		}
	}
	return true;
}

bool Instrumenter::threadJoin(){
	//join
	BPatch_function *pthreadJoin = analyser->getFunction("pthread_join");
	if(pthreadJoin != NULL){
		BPatch_function *cineJoin = analyser->getFunction("cine_join_thread");
		vector<BPatch_point *> *entries = pthreadJoin->findPoint(BPatch_entry);
		vector<BPatch_snippet *> args;
		BPatch_funcCallExpr joinCall(*cineJoin, args);
		return (app->insertSnippet(joinCall, *entries) != NULL);
	}
	return true;
}

bool Instrumenter::threadExit(){
	//exit
	BPatch_function *pthreadExit = analyser->getFunction("pthread_exit");
	if(pthreadExit != NULL){
		BPatch_function *cineExit = analyser->getFunction("cine_exit_thread");
		vector<BPatch_point *> *entries = pthreadExit->findPoint(BPatch_entry);
		vector<BPatch_snippet *> args;
		BPatch_funcCallExpr exitCall(*cineExit, args);
		return (app->insertSnippet(exitCall, *entries) != NULL);
	}
	return true;
}

bool Instrumenter::insertThreadCalls(){

	if(!threadCreation()){
		return false;
	}

	if(!threadStart()){
		return false;
	}

	if(!threadJoin()){
		return false;
	}

	if(!threadExit()){
		return false;
	}

	return true;

	//wrap mutex

}

bool Instrumenter::timeFunction(BPatch_function *f, int methodId){
	vector<BPatch_point *> *entries = f->findPoint(BPatch_entry);
	vector<BPatch_point *> *exits = f->findPoint(BPatch_exit);
	BPatch_function *timerStart = analyser->getFunction("cine_timer_entry");
	BPatch_function *timerStop = analyser->getFunction("cine_timer_exit");
	vector<BPatch_snippet *>args;
	BPatch_constExpr mid(methodId);
	args.push_back(&mid);

	//if you pthread_exit before the end
	vector<BPatch_point *> calls;
	f->getCallPoints(calls);
//	BPatch_flowGraph *graph = f->getCFG();
//	set<BPatch_basicBlock *>blocks;
//	graph->getAllBasicBlocks(blocks);
//	for(set<BPatch_basicBlock *>::const_iterator bi = blocks.begin();
//			bi != blocks.end(); bi++){
//		BPatch_basicBlock *b = *bi;
//		vector<BPatch_edge*> out;
//		b->getIncomingEdges(out);
//		for(vector<BPatch_edge*>::const_iterator o = out.begin();
//				o != out.end(); o++){
//			BPatch_edge *e = *o;
//			cout << e->getTarget()->getFlowGraph()->getFunction()->getName() << endl;
//		}
//	}


	for(vector<BPatch_point *>::const_iterator c = calls.begin();
			c != calls.end(); c++){
		BPatch_point *cp = *c;
		BPatch_function *callee = cp->getCalledFunction();
		if (callee != NULL)
			cout << callee->getName() << endl;
		if(callee != NULL && (callee->getName() == "pthread_exit" || callee->getName() == "exit")){
			cout << "early finish on " << f->getName() << endl;
			exits->push_back(cp);
		}
	}

	cout << f->getName() << " entry points:" << entries->size() << " exit points:" << exits->size() << endl;
	BPatch_funcCallExpr timerStartCall(*timerStart, args);
	BPatch_funcCallExpr timerStopCall(*timerStop, args);
	app->insertSnippet(timerStartCall, *entries, BPatch_lastSnippet);
	app->insertSnippet(timerStopCall, *exits, BPatch_firstSnippet);
	return true;
}



bool Instrumenter::initCalls(){

	//generate a call to VEX::initializeSimulator(NULL)
	BPatch_function * programStart = analyser->getFunction("_start");
	vector<BPatch_point *> *startPoint = programStart->findPoint(BPatch_entry);
	BPatch_function * init = analyser->getFunction("VEX::initializeSimulator");
	vector<BPatch_snippet *> args;
	BPatch_nullExpr null;
	args.push_back(&null);
	BPatch_funcCallExpr initCall(*init, args);

	if(app->insertSnippet(initCall, *startPoint) == NULL){
		return false;
	}

	//register methods
	BPatch_function *reg = analyser->getFunction("cine_method_registration");
	vector<BPatch_function *> ms;
	analyser->getUsefulFunctions(ms);
	int mid = 0;
	for(vector<BPatch_function *>::const_iterator mi = ms.begin();
			mi != ms.end(); mi++){
		BPatch_function *f = *mi;
		vector<BPatch_snippet *> margs;
		const char *n = f->getName().c_str();
		BPatch_constExpr name(n);
		margs.push_back(&name);
		BPatch_constExpr id(mid);
		margs.push_back(&id);

		BPatch_funcCallExpr regCall(*reg, margs);
		if(app->insertSnippet(regCall, *startPoint, BPatch_lastSnippet) == NULL){
			cerr << "method missed " << n << endl;
		}

		timeFunction(f, mid);
		mid++;
	}

	BPatch_function * threadInit = analyser->getFunction("cine_initial_thread");
	vector<BPatch_snippet *> targs;
	BPatch_funcCallExpr threadInitCall(*threadInit, targs);

	if(app->insertSnippet(threadInitCall, *startPoint, BPatch_lastSnippet) == NULL){
		return false;
	}

	return true;

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

bool Instrumenter::instrumentExit() {
	BPatch_function *exit = analyser->getFunction("exit");
	vector<BPatch_point *> *exitPoints;
	if (exit == NULL){
		//try end of main
		exit = analyser->getFunction("main");
		if (exit == NULL){
			return false;
		}
		exitPoints = exit->findPoint(BPatch_exit);
	} else {
		exitPoints = exit->findPoint(BPatch_entry);
	}

	vector<BPatch_snippet *>args;

	BPatch_function *mainExit = analyser->getFunction("cine_exit_thread");
	BPatch_function *cineCleanup = analyser->getFunction("cine_get_results");
	BPatch_funcCallExpr cleanupCall(*cineCleanup, args);
	BPatch_funcCallExpr exitCall(*mainExit, args);

	BPatchSnippetHandle *entrySnippet;

	entrySnippet = app->insertSnippet(exitCall, *exitPoints);

	if (entrySnippet == NULL){
		return false;
	}

	entrySnippet = app->insertSnippet(cleanupCall, *exitPoints, BPatch_lastSnippet);

	return entrySnippet != NULL;
}
