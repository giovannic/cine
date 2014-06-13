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
#include "BPatch_object.h"
#include "BPatch_module.h"
#include "Symtab.h"
#include "Symbol.h"
#include "Command.h"
#include "PatchMgr.h"
#include "debug.h"
using namespace std;
using namespace Dyninst;

Instrumenter::Instrumenter(Analyser *a, BPatch_addressSpace *as){
	analyser = a;
	app = as;
	timers = new vector<BPatchSnippetHandle *>();
	cinemodules = new vector<BPatch_object *>();
	replacemap = new ReplaceMap();
	idMap = new IdMap();
	//cache function calls
}

Instrumenter::~Instrumenter(){
	delete timers;
	delete cinemodules;
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
		DEBUG_PRINT(("one time code failing \n"));
	}
	return false;
}
bool Instrumenter::threadLocklessCreate(){
	BPatch_function *pcreate = analyser->getFunction("pthread_create");
	BPatch_function *cineCreate = analyser->getFunction("cine_lockless_thread_create");

	vector<BPatch_function *>*fs = analyser->getFunctionSet();
	return replaceCalls(*fs, pcreate, cineCreate);
}

bool Instrumenter::threadCreate(){
//
////	BPatch_function *beforeCreate = analyser->getFunction("cine_before_create");
////	BPatch_paramExpr thread(0);
////	vector<BPatch_snippet *> args;
////	args.push_back(&thread);
////	BPatch_funcCallExpr beforeCall(*beforeCreate, args);
////
////	vector<BPatch_point*> *entries = create->findPoint(BPatch_exit);
////	return(app->insertSnippet(beforeCall, *entries) != NULL);
//

	//wrapping version
//	BPatch_function *create = analyser->getFunction("pthread_create");
//	BPatch_function *cineCreate = analyser->getFunction("cine_wrap_thread_create");
//	BPatch_function *replaceCreate = analyser->getFunction("orig_thread_create");
//	return (wrapFunction(create, cineCreate, replaceCreate) &&
//			threadStart());

//	BPatch_function *cineCreate = analyser->getFunction("cine_notify_before_create");
//	vector<BPatch_snippet *>args;
//	BPatch_funcCallExpr createCall(cineCreate, args);
//	vector<BPatch_point *>pts;
//	pcreate->getExitPoints(pts);

	//relocating version
	BPatch_function *pcreate = analyser->getFunction("pthread_create");
	BPatch_function *cineCreate = analyser->getFunction("cine_thread_create");

	vector<BPatch_function *>*fs = analyser->getFunctionSet();
	return replaceCalls(*fs, pcreate, cineCreate);
//	return threadStart();
}

bool Instrumenter::threadCreation(){
//
////	BPatch_function *beforeCreate = analyser->getFunction("cine_before_create");
////	BPatch_paramExpr thread(0);
////	vector<BPatch_snippet *> args;
////	args.push_back(&thread);
////	BPatch_funcCallExpr beforeCall(*beforeCreate, args);
////
////	vector<BPatch_point*> *entries = create->findPoint(BPatch_exit);
////	return(app->insertSnippet(beforeCall, *entries) != NULL);
//

	//wrapping version
//	BPatch_function *create = analyser->getFunction("pthread_create");
//	BPatch_function *cineCreate = analyser->getFunction("cine_wrap_thread_create");
//	BPatch_function *replaceCreate = analyser->getFunction("orig_thread_create");
//	return (wrapFunction(create, cineCreate, replaceCreate) &&
//			threadStart());

//	BPatch_function *cineCreate = analyser->getFunction("cine_notify_before_create");
//	vector<BPatch_snippet *>args;
//	BPatch_funcCallExpr createCall(cineCreate, args);
//	vector<BPatch_point *>pts;
//	pcreate->getExitPoints(pts);

	//relocating version

	return threadCreate() && threadStart();
//	return threadStart();
}

bool Instrumenter::threadDestruction() {
	BPatch_function *fExit = analyser->getFunction("exit");
	BPatch_function *pexit = analyser->getFunction("pthread_exit");
	BPatch_function *cineDestroy = analyser->getFunction("cine_exit_thread");
	BPatch_function *cineDestroyAll = analyser->getFunction("cine_teardown");

	vector<BPatch_function *>*fs;
	fs = analyser->getFunctionSet();
	return (replaceCalls(*fs, pexit, cineDestroy) &&
			replaceCalls(*fs, fExit, cineDestroyAll));
}

bool Instrumenter::mainThreadCreation(){
	BPatch_function *main = analyser->getFunction("main");

	BPatch_function *cineCreate = analyser->getFunction("cine_init");

	vector<BPatch_point *> *entries = main->findPoint(BPatch_entry);

	vector<BPatch_snippet *>args;
	BPatch_funcCallExpr cineSetupCall(*cineCreate, args);

	if(!app->insertSnippet(cineSetupCall, *entries)){
		cerr << "mainthread instrumentation failed" << endl;
	}

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
		DEBUG_PRINT(("entry instrumentation failed \n"));
	} else {
		timers->push_back(entrySnippet);
	}

	DEBUG_PRINT(("instrumented %s\n", entryFunction->getName().c_str()));

	return true;//this can fail
}

//lets hope we are not "clobbering" registers the compiler thinks are untouched
bool Instrumenter::wrapFunction(BPatch_function *f, BPatch_function *newf, BPatch_function *oldf){
	SymtabAPI::Module *symtab = SymtabAPI::convert(oldf->getModule());

	SymtabAPI::Symbol *sym = findSymbol(symtab, oldf->getName());

	if (sym == NULL){
		DEBUG_PRINT(("no replacement function found \n"));
		return false;
	}

	if (!app->wrapFunction(f, newf, sym)){
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
//		DEBUG_PRINT(("start calls: %s\n", (callSite->getCalledFunction() ? callSite->getCalledFunction()->getName().c_str() : string("unknown").c_str())));
		if(callSite->isDynamic()){
			//this is as good as guess as any

//			for(vector<BPatch_edge*>::const_iterator it = outgoing.begin();
//					it != outgoing.end(); it++){
//			}

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
//		if(callSite->getCalledFunction()->getName() == analyser->getFunction("_setjmp")->getName()){
//			vector<BPatch_edge*>outgoing;
//			callSite->getBlock()->getOutgoingEdges(outgoing);
//		}

//		if(callSite->getCalledFunction()->getName() == analyser->getFunction("_setjmp")->getName()){
//			//this is as good as guess as any
//			BPatch_function *cineStart = analyser->getFunction("cine_start_thread");
//			vector<BPatch_snippet *> args;
//			BPatch_funcCallExpr startCall(*cineStart, args);
//			BPatchSnippetHandle *in = app->insertSnippet(startCall, *callSite, BPatch_callBefore, BPatch_firstSnippet);
//			if(in == NULL){
//				return false;
//			}
//		}
//		if(callSite->getCalledFunction()->getName() == analyser->getFunction("__nptl_deallocate_tsd")->getName()){
//			//this is as good as guess as any
//			BPatch_function *cineExit= analyser->getFunction("cine_exit_thread");
//			vector<BPatch_snippet *> args;
//			BPatch_funcCallExpr exitCall(*cineExit, args);
//			BPatchSnippetHandle *out = app->insertSnippet(exitCall, *callSite, BPatch_callAfter, BPatch_lastSnippet);
//			if(out == NULL){
//				return false;
//			}
//		}
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

bool Instrumenter::replaceCalls(vector<BPatch_function *> &fs,
		vector<BPatch_function *>&oldFs, BPatch_function *newF){
	for (vector<BPatch_function *>::const_iterator it = oldFs.begin();
			it != oldFs.end(); ++it){
		BPatch_function *f = *it;
		if(!replaceCalls(fs, f, newF)){
			return false;
		}
	}
	return true;
}

bool Instrumenter::replaceCalls(vector<BPatch_function *> &fs,
		BPatch_function *oldF, BPatch_function *newF){
	vector<BPatch_point *> callPoints;
	if(oldF == NULL){
		return true;
	}

	for(vector<BPatch_function *>::const_iterator it = fs.begin();
			it != fs.end(); it++){
		BPatch_function *f = *it;
		analyser->getCalls(f, oldF, callPoints);
	}

	for(vector<BPatch_point *>::const_iterator it = callPoints.begin();
			it != callPoints.end(); it++){
		BPatch_point *p = *it;
		if(!app->replaceFunctionCall(*p, *newF)){
			return false;
		}
	}
	return true;
}

bool Instrumenter::instrumentSleep() {
	BPatch_function *sleep = analyser->getFunction("sleep");
	BPatch_function *cineSleep = analyser->getFunction("cine_sleep");

	BPatch_function *yield = analyser->getFunction("pthread_yield");
	BPatch_function *cineYield = analyser->getFunction("cine_yield");

	BPatch_function *join = analyser->getFunction("pthread_join");
	BPatch_function *cineJoin = analyser->getFunction("cine_join");

	vector<BPatch_function *> *fs;
	fs = analyser->getFunctionSet();

	return (replaceCalls(*fs, sleep, cineSleep) &&
			replaceCalls(*fs, yield, cineYield) &&
			replaceCalls(*fs, join, cineJoin)
			);

}

bool Instrumenter::threadMutex(){
	//pthread_mutex_trylock!!
	//Hack to true

	//pthread_cond_wait
	//pthread_cond_timed_wait

	vector<BPatch_function *> *fs;
	fs = analyser->getFunctionSet();

	//this will regex search and find the mangled name
	BPatch_function *mutexLock = analyser->getFunction("pthread_mutex_lock");
	BPatch_function *cineMutexLock = analyser->getFunction("cine_mutex_lock");

	BPatch_function *mutexUnlock = analyser->getFunction("pthread_mutex_unlock");
	BPatch_function *cineMutexUnlock = analyser->getFunction("cine_mutex_unlock");

	vector<BPatch_function *>condWaits = analyser->getAllFunctions("pthread_cond_wait");
	BPatch_function *cineCondWait = analyser->getFunction("cine_cond_wait");

	vector<BPatch_function *>condTimedwaits = analyser->getAllFunctions("pthread_cond_timedwait");
	BPatch_function *cineCondTimedwait = analyser->getFunction("cine_cond_timedwait");

	vector<BPatch_function *>condSignals = analyser->getAllFunctions("pthread_cond_signal");
	BPatch_function *cineCondSignal = analyser->getFunction("cine_cond_signal");

	vector<BPatch_function *>condBroadcasts = analyser->getAllFunctions("pthread_cond_broadcast");
	BPatch_function *cineCondBroadcast = analyser->getFunction("cine_cond_broadcast");

	return (replaceCalls(*fs, mutexLock, cineMutexLock) &&
			replaceCalls(*fs, mutexUnlock, cineMutexUnlock) &&
			replaceCalls(*fs, condWaits, cineCondWait) &&
			replaceCalls(*fs, condTimedwaits, cineCondTimedwait) &&
			replaceCalls(*fs, condSignals, cineCondSignal) &&
			replaceCalls(*fs, condBroadcasts, cineCondBroadcast)
			);

	//attempt to wrap
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

	if(!threadMutex()){
		DEBUG_PRINT(("mutexes failed \n"));
	}

	if(!threadExit()){
		DEBUG_PRINT(("no exit call \n"));
	}

	return true;

}

bool Instrumenter::timeFunctionCalls(BPatch_function *f, int methodId){

	vector<BPatch_function *>*useful;
	vector<BPatch_point *>callpts;
	useful = analyser->getFunctionSet();
	for(vector<BPatch_function *>::const_iterator it = useful->begin();
			it != useful->end(); ++it){
		BPatch_function *usefulF = *it;
		analyser->getCalls(usefulF, f, callpts);
	}

	BPatch_function *fAllExit = analyser->getFunction("exit");
	BPatch_function *fPExit = analyser->getFunction("pthread_exit");

	BPatch_function *timerStart = analyser->getFunction("cine_timer_entry");
	BPatch_function *timerStop = analyser->getFunction("cine_timer_exit");
	vector<BPatch_snippet *>args;
	BPatch_constExpr mid(methodId);
	args.push_back(&mid);

	BPatch_funcCallExpr timerStartCall(*timerStart, args);
	BPatch_funcCallExpr timerStopCall(*timerStop, args);

	BPatch_point *allExit = analyser->hasCall(f, fAllExit);
	if(allExit != NULL){
		app->insertSnippet(timerStopCall, *allExit, BPatch_callBefore, BPatch_firstSnippet);
	}
	BPatch_point *pExit = analyser->hasCall(f, fPExit);
	if(pExit != NULL){
		app->insertSnippet(timerStopCall, *pExit, BPatch_callBefore, BPatch_firstSnippet);
	}

	if(allExit || pExit){
		cout << "early finish on " << f->getName() << endl;
		DEBUG_PRINT(("early finish on %s\n", f->getName().c_str()));
	}

	BPatchSnippetHandle *s = app->insertSnippet(timerStartCall, callpts, BPatch_callBefore, BPatch_lastSnippet);
	BPatchSnippetHandle *e = app->insertSnippet(timerStopCall, callpts, BPatch_callAfter, BPatch_firstSnippet);
	return (s !=NULL && e != NULL);
}

bool Instrumenter::timeFunctionInvalidating(BPatch_function *f, int methodId){
	BPatch_function *timerStart = analyser->getFunction("cine_timer_entry");
	BPatch_function *timerStop = analyser->getFunction("cine_timer_invalidate_exit");
	return timeFunction(f, methodId, timerStart, timerStop);
}

bool Instrumenter::timeFunction(BPatch_function *f, int methodId){
	BPatch_function *timerStart = analyser->getFunction("cine_timer_entry");
	BPatch_function *timerStop = analyser->getFunction("cine_timer_exit");
	return timeFunction(f, methodId, timerStart, timerStop);
}

bool Instrumenter::timeFunction(BPatch_function *f, int methodId,
		BPatch_function *timerStart, BPatch_function *timerStop){

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
	if(analyser->toIgnore->find(f) != analyser->toIgnore->end()){
		cout << f->getName() << " is filtering through" << endl;
	}

	vector<BPatch_point *> *entries = f->findPoint(BPatch_entry);
	vector<BPatch_point *> *exits = f->findPoint(BPatch_exit);

	BPatch_function *fExit = analyser->getFunction("exit");
	BPatch_function *fPExit = analyser->getFunction("pthread_exit");

	vector<BPatch_snippet *>args;
	BPatch_constExpr mid(methodId);
	args.push_back(&mid);

	//if you pthread_exit before the end

	BPatch_point *allExit;
	if(fExit && (allExit = analyser->hasCall(f, fExit)) != NULL){
		exits->push_back(allExit);
	}
	BPatch_point *pExit;
	if(fPExit && (pExit = analyser->hasCall(f, fPExit)) != NULL){
		exits->push_back(pExit);
	}
	if(allExit || pExit){
		DEBUG_PRINT(("early finish on %s\n", f->getName().c_str()));
	}

	void *beg;
	void *end;
	f->getAddressRange(beg, end);
	DEBUG_PRINT(("%s ranges %p to %p\n", f->getName().c_str(), beg, end));
	for(vector<BPatch_point *>::const_iterator it = entries->begin();
			it != entries->end(); it ++){
		BPatch_point *p = *it;
	}

	for(vector<BPatch_point *>::const_iterator it = exits->begin();
			it != exits->end(); it ++){
		BPatch_point *p = *it;
	}

	BPatch_funcCallExpr timerStartCall(*timerStart, args);
	BPatch_funcCallExpr timerStopCall(*timerStop, args);

	BPatchSnippetHandle *s = app->insertSnippet(timerStartCall, *entries, BPatch_lastSnippet);
	BPatchSnippetHandle *e = app->insertSnippet(timerStopCall, *exits, BPatch_firstSnippet);
	return (s !=NULL && e != NULL);
}

bool Instrumenter::addPolicy(string& policy){
	BPatch_function *ireg = analyser->getFunction("cine_invalidation_registration");

	BPatch_function * programStart = analyser->getFunction("_start");
	vector<BPatch_point *> *startPoint = programStart->findPoint(BPatch_entry);
	bool err;
	vector<BPatch_snippet *> margs;
	const char *n = policy.c_str();
	BPatch_constExpr name(n);
	margs.push_back(&name);

	BPatch_funcCallExpr iregCall(*ireg, margs);
	if(app->insertSnippet(iregCall, *startPoint, BPatch_lastSnippet) == NULL){
		DEBUG_PRINT(("invalidation cannot be registered \n"));
	}
	if(err){
		return false;
	}

	return true;
}

bool Instrumenter::staticInitCalls(){
	BPatch_function * init = analyser->getFunction("cine_static_init");
	initCalls(init);
}

bool Instrumenter::initCalls(){
	BPatch_function * init = analyser->getFunction("cine_init");
	initCalls(init);
}

bool Instrumenter::initCalls(BPatch_function *init){

	//generate a call to VEX::initializeSimulator(NULL)
	BPatch_function * programStart = analyser->getFunction("_start");
//	BPatch_function * programStart = analyser->getFunction("main");
	vector<BPatch_point *> *startPoint = programStart->findPoint(BPatch_entry);
	vector<BPatch_snippet *> args;
	BPatch_nullExpr null;
	args.push_back(&null);
	BPatch_funcCallExpr initCall(*init, args);

	if(app->insertSnippet(initCall, *startPoint, BPatch_firstSnippet) == NULL){
		return false;
	}


	//cine does this now
//	BPatch_function * threadInit = analyser->getFunction("cine_initial_thread");
//	vector<BPatch_snippet *> targs;
//	BPatch_funcCallExpr threadInitCall(*threadInit, targs);
//
//	if(app->insertSnippet(threadInitCall, *startPoint, BPatch_lastSnippet) == NULL){
//		return false;
//	}

	return true;

}

bool Instrumenter::registerMethods(SpeedupMap &speedups){
		//register methods
	BPatch_function *reg = analyser->getFunction("cine_method_registration");
	vector<BPatch_function *> *ms;
	ms = analyser->getUsefulFunctions();
	int mid = 0;
	for(vector<BPatch_function *>::const_iterator mi = ms->begin();
			mi != ms->end(); mi++){
		BPatch_function *f = *mi;
		vector<BPatch_snippet *> margs;
		const char *n = f->getName().c_str();
		BPatch_constExpr name(n);
		margs.push_back(&name);
		BPatch_constExpr id(mid);
		margs.push_back(&id);

		BPatch_funcCallExpr regCall(*reg, margs);
		if(app->insertSnippet(regCall, *analyser->getStartPoint(), BPatch_lastSnippet) == NULL){
			DEBUG_PRINT(("method cannot be registered \n"));
		}

		map<BPatch_function*,long>::iterator record;
		if((record = speedups.find(f)) != speedups.end()){
			registerSpeedup(mid, (*record).second);
		}

		(*idMap)[mid] = f;
		if(!timeFunction(f, mid)){
			DEBUG_PRINT(("method cannot be timed\n"));
		}
		mid++;
	}
	return true;
}

bool Instrumenter::instrumentLibraries(){
//	vector<BPatch_module *>ms;
//	for (vector<BPatch_object*>::const_iterator it = cinemodules->begin();
//			it != cinemodules->end(); it++){
//		BPatch_object *o = *it;
//		o->modules(ms);
//
//		for (vector<BPatch_module*>::const_iterator im = ms.begin();
//			im != ms.end(); im++){
//			BPatch_module *m = *im;
//			vector<BPatch_function*>mfs =  m->getProcedures();
//			for (vector<BPatch_function*>::const_iterator itf = mfs.begin();
//					itf != mfs.end(); itf++){
//				vector<BPatch_point*>calls;
//				(*itf)->getCallPoints(calls);
//				for (vector<BPatch_point*>::const_iterator call = calls.begin();
//						call != calls.end(); call++){
//					manageCall(*call);
//				}
//			}
//		}
//	}
	return false;
}

bool Instrumenter::loadLibraries(){

	BPatch_object *lib;

	if((lib = app->loadLibrary("libvex.so")) == NULL){
		cerr << "target loading of vex failed" << endl;
		return false;
	}
	cinemodules->push_back(lib);

	if((lib = app->loadLibrary("libcine.so")) == NULL){
		cerr << "target loading of cine failed" << endl;
		return false;
	}
	cinemodules->push_back(lib);

	return true;
}

bool Instrumenter::wrapUp(vector<BPatch_point *> *exitPoints){
	vector<BPatch_snippet *>args;
	BPatch_function *cineCleanup = analyser->getFunction("cine_exit_thread");
	BPatch_funcCallExpr cleanupCall(*cineCleanup, args);

	//should exit all threads -> vex stuff
//	BPatch_function *mainExit = analyser->getFunction("cine_exit_thread");
//	BPatch_funcCallExpr exitCall(*mainExit, args);
//
//	BPatchSnippetHandle *entrySnippet = app->insertSnippet(exitCall, *exitPoints, BPatch_lastSnippet);
//
//	if (entrySnippet == NULL){
//		return false;
//	}

	return (app->insertSnippet(cleanupCall, *exitPoints, BPatch_lastSnippet) != NULL);
}

bool Instrumenter::finalFunction(string f){
	vector<BPatch_point *> *exitPoints;
	BPatch_function *exit = analyser->getFunction(f);
	if (exit == NULL){
		return false;
	}

	exitPoints = exit->findPoint(BPatch_exit);

	//unnecessary
//	string exitName = "exit";
//
//	BPatch_point *p = analyser->hasCall(exit, exitName);
//	if (p != NULL){
//		exitPoints->push_back(p);
//	}

	if (exitPoints->size() == 0){
		return false;
	}
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

bool Instrumenter::instrumentContention() {
	return (threadMutex() && instrumentSleep());
}

//doesn't work
bool Instrumenter::debugStart() {
	BPatch_function *start = analyser->getFunction("start_thread");
	vector<BPatch_point *> possible;
	start->getCallPoints(possible);

	for (vector<BPatch_point *>::const_iterator pi = possible.begin();
			pi != possible.end(); pi++){
		BPatch_point *callSite = *pi;
		if(callSite->isDynamic()){
			//this is as good a guess as any
			BPatch_function *debugger = analyser->getFunction("print_address");
			vector<BPatch_snippet *> args;
			BPatch_dynamicTargetExpr startF;
			args.push_back(&startF);
			BPatch_funcCallExpr startCall(*debugger, args);
			BPatchSnippetHandle *in = app->insertSnippet(startCall, *callSite, BPatch_callBefore, BPatch_firstSnippet);
			return (in == NULL);
		}
	}
	return false;
}

bool Instrumenter::loadPthreads() {
//this segfaults dyninst in dynamic mode
	if(!app->loadLibrary("libpthread.so.0")){
		cerr << "target loading of pthread failed" << endl;
		return false;
	}
	return true;
}

bool Instrumenter::time() {
	vector<BPatch_function *> *fs;
	fs = analyser->getFunctionSet();
	BPatch_function *time = analyser->getFunction("time");
	BPatch_function *cineTime = analyser->getFunction("cine_time");
	if(time == NULL){
		return false;
	}
	return replaceCalls(*fs, time, cineTime);
}

void Instrumenter::manageCall(BPatch_point* call) {
//	if(call->getCalledFunction())
}

bool Instrumenter::registerSpeedup(int mid, long speedup) {
	BPatch_function *reg = analyser->getFunction("cine_speedup_registration");
	vector<BPatch_snippet *> margs;
	BPatch_constExpr id(mid);
	margs.push_back(&id);
	BPatch_constExpr speeduparg((long)speedup);
	margs.push_back(&speeduparg);
}

bool Instrumenter::loadDyninst() {
	if(!app->loadLibrary("libdyninstAPI_RT.so.8.2.0")){
		cerr << "reloading of dyninst failed" << endl;
		return false;
	}
	return true;
}
//void Instrumenter::patchUserMessage() {
//	BPatch_function *invalidate = analyser->getFunction("cine_timer_invalidate_exit");
//}
