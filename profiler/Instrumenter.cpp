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
	BPatch_function * m = analyser->getFunction("main");
	BPatch_function *cineCreate = analyser->getFunction("cine_initial_thread");
	BPatch_function *cineDestroy = analyser->getFunction("cine_exit_thread");
	return instrumentThreadEntry(m, cineCreate, cineDestroy);
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

bool Instrumenter::insertThreadCalls(){
	//doesn't work
//	BPatch_function *start = analyser->getFunction("start_thread");
//	return instrumentThreadEntry(start);

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

	//join
	BPatch_function *pthreadJoin = analyser->getFunction("pthread_join");
	if(pthreadJoin){
        BPatch_function *cineJoin = analyser->getFunction("cine_join_thread");
        vector<BPatch_point *> *entries = pthreadJoin->findPoint(BPatch_entry);
        vector<BPatch_snippet *> args;
        BPatch_funcCallExpr joinCall(*cineJoin, args);
        if (!app->insertSnippet(joinCall, *entries)){
                return false;
        }
	}


	//exit
	BPatch_function *pthreadExit = analyser->getFunction("pthread_exit");
	if(pthreadExit != NULL){
		BPatch_function *cineExit = analyser->getFunction("cine_exit_thread");
		vector<BPatch_point *> *entries = pthreadExit->findPoint(BPatch_entry);
		vector<BPatch_snippet *> args;
		BPatch_funcCallExpr exitCall(*cineExit, args);
		if (!app->insertSnippet(exitCall, *entries)){
			return false;
		}
	}


	return true;

	//join
	//wrap mutex

	/*
	-	vector<BPatch_snippet*>args;
	-	BPatch_funcCallExpr lfCall(*cineCreate, args);
	-
	-	//TODO: find the right point
	-	return app->insertSnippet(lfCall,
	-			*create->findPoint(BPatch_locEntry),
	-			BPatch_callBefore);
	-			*/
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
	for(vector<BPatch_point *>::const_iterator c = calls.begin();
			c != calls.end(); c++){
		BPatch_point *cp = *c;
		BPatch_function *callee = cp->getCalledFunction();
		if(callee != NULL && callee->getName() == "pthread_exit"){
			exits->push_back(cp);
		}
	}

	BPatch_funcCallExpr timerStartCall(*timerStart, args);
	BPatch_funcCallExpr timerStopCall(*timerStop, args);
	app->insertSnippet(timerStartCall, *entries, BPatch_lastSnippet);
	app->insertSnippet(timerStopCall, *exits, BPatch_firstSnippet);
	return true;
}

bool Instrumenter::beginSimulator(BPatch_process *p){
	//generate a call to VEX::initializeSimulator(NULL)
	BPatch_function * init = analyser->getFunction("VEX::initializeSimulator");
	vector<BPatch_snippet *> args;
	BPatch_nullExpr null;
	args.push_back(&null);
	BPatch_funcCallExpr initCall(*init, args);
	bool err;
	void * success = p->oneTimeCode(initCall, &err);

	if(err){
		cerr << " did not initialise " << endl;
	}

	//register methods
	BPatch_function *reg = analyser->getFunction("cine_method_registration");
	vector<BPatch_function *> ms;
	analyser->getUsefulFunctions(ms);
	int mid = 0;
	for(vector<BPatch_function *>::const_iterator mi = ms.begin();
			mi != ms.end(); mi++){
		BPatch_function *f = *mi;
		vector<BPatch_snippet *> args;
		const char *n = f->getName().c_str();
		BPatch_constExpr name(n);
		args.push_back(&name);
		BPatch_constExpr id(mid);
		args.push_back(&id);

		BPatch_funcCallExpr regCall(*reg, args);
		p->oneTimeCode(regCall, &err);

		timeFunction(f, mid);
		mid++;
	}

//	vector<BPatch_thread *>ts;
//	p->getThreads(ts);

	//blocks for some reason
//	BPatch_function * initThread = analyser->getFunction("ThreadEventsBehaviour::onThreadMainStart");
	BPatch_function * initThread = analyser->getFunction("cine_initial_thread");
	vector<BPatch_snippet *> tArgs;

//	BPatch_threadIndexExpr tid;
//	tArgs.push_back(&tid);

	BPatch_funcCallExpr initThreadCall(*initThread, tArgs);

	p->oneTimeCode(initThreadCall, &err);
	if(err){
		cerr << " did not notify of main thread " << endl;
	}
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

bool Instrumenter::instrumentCreation() {
	//Try before creation after creation
	return true;
}

bool Instrumenter::instrumentExit() {
	BPatch_function *exit = analyser->getFunction("exit");
	vector<BPatch_point *> *entries = exit->findPoint(BPatch_entry);

	vector<BPatch_snippet *>args;

	BPatch_function *mainExit = analyser->getFunction("cine_exit_thread");
	BPatch_function *cineCleanup = analyser->getFunction("cine_get_results");
	BPatch_funcCallExpr cleanupCall(*cineCleanup, args);
	BPatch_funcCallExpr exitCall(*mainExit, args);

	BPatchSnippetHandle *entrySnippet;

	entrySnippet = app->insertSnippet(exitCall, *entries);

	if (entrySnippet == NULL){
		return false;
	}

	entrySnippet = app->insertSnippet(cleanupCall, *entries, BPatch_lastSnippet);

	return entrySnippet != NULL;
}
