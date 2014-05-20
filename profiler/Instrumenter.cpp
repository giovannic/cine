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
#include "Command.h"
#include "PatchMgr.h"
using namespace std;
using namespace Dyninst;

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
SymtabAPI::Symbol *findSymbol(SymtabAPI::Module *m, string s){
	vector <SymtabAPI::Symbol *> syms;
	//get the method names
	m->getAllSymbols(syms);
	for( std::vector<SymtabAPI::Symbol *>::const_iterator i = syms.begin();
			i != syms.end(); ++i){
		SymtabAPI::Symbol *f = *i;
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
	return true;

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

//lets hope we are not "clobbering" registers the compiler thinks are untouched
bool Instrumenter::wrapFunction(BPatch_function *f, BPatch_function *newf, BPatch_function *oldf){
	SymtabAPI::Module *symtab = SymtabAPI::convert(oldf->getModule());

	SymtabAPI::Symbol *sym = findSymbol(symtab, oldf->getName());

	if (sym == NULL){
		cerr << "no replacement functions found" << endl;
		return false;
	}

	if (!app->wrapFunction(f, newf, sym)){
		return false;
	}
	return true;
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
	return wrapFunction(create, cineCreate, replaceCreate);

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
			BPatch_function *cineExit= analyser->getFunction("cine_exit_thread");
			vector<BPatch_snippet *> args;
			BPatch_funcCallExpr startCall(*cineStart, args);
			BPatch_funcCallExpr exitCall(*cineExit, args);
			BPatchSnippetHandle *in = app->insertSnippet(startCall, *callSite, BPatch_callBefore, BPatch_firstSnippet);
			BPatchSnippetHandle *out = app->insertSnippet(exitCall, *callSite, BPatch_callAfter, BPatch_lastSnippet);
			if(in == NULL || out == NULL){
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

bool Instrumenter::threadMutex(){

	BPatch_function *mutexLock = analyser->getFunction("pthread_mutex_lock");
	BPatch_function *cineMutexLock = analyser->getFunction("cine_mutex_lock");

	vector<BPatch_point *> callers;
	mutexLock->getCallerPoints(callers);

	cout << callers.size() << " callers of pthread_mutex_lock" << endl;
	for(vector<BPatch_point *>::const_iterator it = callers.begin();
			it != callers.end(); it++){
		BPatch_point *p = *it;
		cout << p->getFunction()->getName() << " calls pthread_mutex_lock" << endl;
		if(!p->getFunction()->getModule()->isSharedLib()){
			if(!app->replaceFunctionCall(*p, *cineMutexLock)){
				cout << "failed to replace function call" << endl;
			}
		}
	}

//	vector<BPatch_function *> fs;
//	analyser->getUsefulFunctions(fs);
//
//	for(vector<BPatch_function *>::const_iterator it = fs.begin();
//			it != fs.end(); it++){
//		BPatch_function *f = *it;
//
//	}
//	BPatch_function *mutexLock = analyser->getFunction("pthread_mutex_lock");
//	PatchAPI::PatchMgrPtr mgr = PatchAPI::convert(app);
//	PatchAPI::PatchFunction *pf = PatchAPI::convert(mutexLock);


//mgr->findPoints(PatchAPI::Scope(pf),
//			PatchAPI::Point::FuncEntry,
//			back_inserter(entrypts));
//
//	mgr->findPoints(PatchAPI::Scope(pf),
//			PatchAPI::Point::FuncExit,
//			back_inserter(exitpts));

//	BPatch_function *mutexLock = analyser->getFunction("pthread_mutex_lock");
//	BPatch_function *cineMutexLock = analyser->getFunction("cine_mutex_lock");
//	BPatch_function *cineMutexLockOrig = analyser->getFunction("orig_pthread_mutex_lock");
//
//	BPatch_function *mutexUnlock = analyser->getFunction("pthread_mutex_unlock");
//	BPatch_function *cineMutexUnlock = analyser->getFunction("cine_mutex_unlock");
//	BPatch_function *cineMutexUnlockOrig = analyser->getFunction("orig_pthread_mutex_unlock");
//
//	//pthread_mutex_trylock!!
//
//	if(mutexLock != NULL && !wrapFunction(mutexLock, cineMutexLock, cineMutexLockOrig)){
//		return false;
//	}
//
//	if(mutexUnlock != NULL && !wrapFunction(mutexUnlock, cineMutexUnlock, cineMutexUnlockOrig)){
//		return false;
//	}

	//make vex call orig...

	/*
	if(pthreadMutex != NULL){
		BPatch_function *cine= analyser->getFunction("cine_join_thread");
		vector<BPatch_point *> *entries = pthreadJoin->findPoint(BPatch_entry);
		vector<BPatch_snippet *> args;
		BPatch_funcCallExpr joinCall(*cineJoin, args);
		return (app->insertSnippet(joinCall, *entries) != NULL);
	}
	*/
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
		cout << "no joins found" << endl;
	}

	if(!threadMutex()){
		cout << "no mutexes found" << endl;
	}

	if(!threadExit()){
		cout << "threads exit naturally" << endl;
	}

	return true;

	//wrap mutex

}

bool Instrumenter::timeFunction(BPatch_function *f, int methodId){

//	PatchAPI::PatchMgrPtr mgr = PatchAPI::convert(app);
//	PatchAPI::PatchFunction *pf = PatchAPI::convert(f);
//
//	vector<PatchAPI::Point *> entrypts;
//	vector<PatchAPI::Point *> exitpts;
//	mgr->findPoints(PatchAPI::Scope(pf),
//			PatchAPI::Point::FuncEntry,
//			back_inserter(entrypts));
//
//	mgr->findPoints(PatchAPI::Scope(pf),
//			PatchAPI::Point::FuncExit,
//			back_inserter(exitpts));
//
//	vector<BPatch_snippet *>args;
//	BPatch_constExpr mid(methodId);
//	args.push_back(&mid);
//	BPatch_function *timerStart = analyser->getFunction("cine_timer_entry");
//	BPatch_function *timerStop = analyser->getFunction("cine_timer_exit");
//	BPatch_funcCallExpr timerStartCall(*timerStart, args);
//	BPatch_funcCallExpr timerStopCall(*timerStop, args);
//
//	cout << f->getName() << " entry points: " << entrypts.size() << endl;
//	cout << f->getName() << " exit points: " << exitpts.size() << endl;
//
//	PatchAPI::Snippet::Ptr ptimerStart = (PatchAPI::Snippet::Ptr) timerStartCall.ast_wrapper; //PatchAPI::convert(&timerStartCall);
//	PatchAPI::Snippet::Ptr ptimerStop = (PatchAPI::Snippet::Ptr) timerStopCall.ast_wrapper; // PatchAPI::convert(&timerStopCall);
//
//
//	PatchAPI::Patcher patcher(mgr);
//	for (vector<PatchAPI::Point *>::iterator i = entrypts.begin();
//			i != entrypts.end(); ++i){
//		patcher.add(PatchAPI::PushBackCommand::create(*i, ptimerStart));
//	}
//
//	for (vector<PatchAPI::Point *>::iterator i = exitpts.begin();
//			i != exitpts.end(); ++i){
//		patcher.add(PatchAPI::PushFrontCommand::create(*i, ptimerStop));
//	}
//
//	return patcher.commit();

	vector<BPatch_point *> *entries = f->findPoint(BPatch_entry);
	vector<BPatch_point *> *exits = f->findPoint(BPatch_exit);
	BPatch_function *timerStart = analyser->getFunction("cine_timer_entry");
	BPatch_function *timerStop = analyser->getFunction("cine_timer_exit");
	vector<BPatch_snippet *>args;
	BPatch_constExpr mid(methodId);
	args.push_back(&mid);

	//if you pthread_exit before the end
	vector<BPatch_point *> *callptr = f->findPoint(BPatch_subroutine);
//	f->getCallPoints(calls);

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

	vector<BPatch_point *>calls = *callptr;

//	cout << "calls found: " << calls.size() << endl;
	for(vector<BPatch_point *>::const_iterator c = calls.begin();
			c != calls.end(); c++){
		BPatch_point *cp = *c;
		BPatch_function *callee = cp->getBlock()->getCallTarget();
//		BPatch_function *callee = cp->getCalledFunction();
		if (callee != NULL)
//			cout << callee->getName() << endl;
		if(callee != NULL && (callee->getName() == "pthread_exit" || callee->getName() == "exit")){
			cout << "early finish on " << f->getName() << endl;
			exits->push_back(cp);
		}
	}

//	cout << f->getName() << " entry points: " << entries->front()->getAddress() << " exit points:" << exits->size() << endl;
	BPatch_funcCallExpr timerStartCall(*timerStart, args);
	BPatch_funcCallExpr timerStopCall(*timerStop, args);

	BPatchSnippetHandle *s = app->insertSnippet(timerStartCall, *entries, BPatch_lastSnippet);
	BPatchSnippetHandle *e = app->insertSnippet(timerStopCall, *exits, BPatch_firstSnippet);
	return (s !=NULL && e != NULL);
}



bool Instrumenter::initCalls(){

	//generate a call to VEX::initializeSimulator(NULL)
	BPatch_function * programStart = analyser->getFunction("_start");
	vector<BPatch_point *> *startPoint = programStart->findPoint(BPatch_entry);
	BPatch_function * init = analyser->getFunction("cine_init");
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
			cerr << "method cannot be registered" << n << endl;
		}

		if(!timeFunction(f, mid)){
			cerr << " method cannot be timed" << endl;
		}
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

bool Instrumenter::wrapUp(vector<BPatch_point *> *exitPoints){
	vector<BPatch_snippet *>args;
	BPatch_function *cineCleanup = analyser->getFunction("cine_get_results");
	BPatch_funcCallExpr cleanupCall(*cineCleanup, args);

	//should exit all threads -> vex stuff
	BPatch_function *mainExit = analyser->getFunction("cine_exit_thread");
	BPatch_funcCallExpr exitCall(*mainExit, args);

	BPatchSnippetHandle *entrySnippet = app->insertSnippet(exitCall, *exitPoints, BPatch_lastSnippet);

	if (entrySnippet == NULL){
		return false;
	}

	return (app->insertSnippet(cleanupCall, *exitPoints, BPatch_lastSnippet) != NULL);
}

bool Instrumenter::finalFunction(string f){
	vector<BPatch_point *> *exitPoints;
	BPatch_function *exit = analyser->getFunction(f);
	if (exit == NULL){
		return false;
	}
	exitPoints = exit->findPoint(BPatch_exit);
	return wrapUp(exitPoints);
}

bool Instrumenter::instrumentExit() {
	BPatch_function *exit = analyser->getFunction("exit");
	vector<BPatch_point *> *exitPoints;
	if (exit == NULL){
		return false;
	}
	exitPoints = exit->findPoint(BPatch_entry);

	return wrapUp(exitPoints);

}
