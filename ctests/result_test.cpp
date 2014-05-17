/*
 * instrument_test.c
 *
 *  Created on: 5 Feb 2014
 *      Author: giovanni
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Launcher.h"
#include "Controller.h"
#include "Instrumenter.h"
#include "Analyser.h"
#include <unistd.h>
#include <iostream>
#include <sstream>

using namespace std;

TEST_CASE( "simple threads", "[instruments]" ) {

	//basic
	string testpath(BIN_PATH);
	string threadfile = testpath + "/mutexthreads";
	Launcher l(&threadfile);

	BPatch_addressSpace *app = l.openBinary();
	REQUIRE(app != NULL);

	Analyser a(app->getImage());
	Instrumenter inst(&a, app->getImage()->getAddressSpace());

//	SECTION("just methods and sequential threads"){
		REQUIRE(inst.loadLibraries());
		//REQUIRE(inst.instrumentMain());

		REQUIRE(inst.initCalls());
//		REQUIRE(inst.insertThreadCalls());
		REQUIRE(inst.instrumentExit());
//		Controller c(&inst, &a, app);
//		c.listenThreads();
		//refactor
//		l.listenResults();

		//somehow check
		l.launch();
//	}

}

//TEST_CASE( "mutex threads", "[instruments]" ) {
//
//	//basic
//	string testpath(BIN_PATH);
//	string threadfile = testpath + "/mutexthreads";
//	Launcher l(&threadfile);
//
//	BPatch_process *app = l.createProcess();
//	REQUIRE(app != NULL);
//
//	Analyser a(app->getImage());
//	Instrumenter inst(&a, app->getImage()->getAddressSpace());
//
////	SECTION("just methods and sequential threads"){
//		REQUIRE(inst.loadLibraries());
//		//REQUIRE(inst.instrumentMain());
//		REQUIRE(inst.beginSimulator(app));
//		REQUIRE(inst.insertThreadCalls());
//		REQUIRE(inst.instrumentExit());
//
//		Controller c(&inst, &a, app);
//		c.listenThreads();
//		//refactor
////		l.listenResults();
//		vector <BPatch_function *> fs;
//		a.getUsefulFunctions(fs);
//
//		int mid = 1;
//		for (vector <BPatch_function *>::iterator it = fs.begin();
//				it != fs.end(); it++){
//			BPatch_function *f = *it;
//			inst.timeFunction(f, mid++);
//		}
//
//		//REQUIRE(inst.insertThreadCalls());
//		//somehow check
//		l.launch();
////	}
//
//}
