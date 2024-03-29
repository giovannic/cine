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

TEST_CASE( "Pthreads", "[instruments]" ) {

	//basic
	string testpath(BIN_PATH);
	string threadfile = testpath + "/threads";
	BPatch *bpatch = new BPatch();

	const char *f = threadfile.c_str();
	BPatch_binaryEdit *app = bpatch->openBinary(f);
	REQUIRE(app != NULL);

	Analyser a(app->getImage());
	Instrumenter inst(&a, app->getImage()->getAddressSpace());

	SECTION("pthread analysis"){
		REQUIRE(a.getFunction("pthread_exit") != NULL);
		REQUIRE(a.getFunction("pthread_exit") != NULL);
	}

	SECTION("check library functions exist"){
		REQUIRE(inst.loadLibraries());
		BPatch_module *vex = app->getImage()->findModule("libvex.so", false);
		BPatch_module *cine = app->getImage()->findModule("libcine.so", false);
		REQUIRE(vex != NULL);
		REQUIRE(cine != NULL);

		//cine
		REQUIRE(a.getFunction("cine_thread_create") != NULL);
		REQUIRE(a.getFunction("orig_thread_create") != NULL);

		//vex
		REQUIRE(a.getFunction("ThreadEventsBehaviour::onStart") != NULL);
		REQUIRE(a.getFunction("ThreadEventsBehaviour::onEnd") != NULL);
	}

	//insert more here
	SECTION("time methods"){
		REQUIRE(inst.loadLibraries());
		vector <BPatch_function *> *fs;
		fs = a.getUsefulFunctions();

		int mid = 1;
		for (vector <BPatch_function *>::iterator it = fs->begin();
				it != fs->end(); it++){
			BPatch_function *f = *it;
			//cout << mid << "/" << fs.size() << endl;
			inst.timeFunction(f, mid++);

		}

		//REQUIRE(inst.insertThreadCalls());
		//somehow check
	}

	//insert more here
	SECTION("control pthreads"){
		REQUIRE(inst.loadLibraries());
		REQUIRE(inst.initCalls());
		//REQUIRE(inst.instrumentContention());

		//REQUIRE(inst.insertThreadCalls());
		//somehow check
	}

	delete bpatch;


}
