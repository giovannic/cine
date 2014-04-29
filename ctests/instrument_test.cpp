/*
 * instrument_test.c
 *
 *  Created on: 5 Feb 2014
 *      Author: giovanni
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Launcher.h"
#include "Instrumenter.h"
#include <unistd.h>
#include <iostream>
#include <sstream>

using namespace std;

TEST_CASE( "Pthreads", "[instruments]" ) {

	//basic
	string testpath(BIN_PATH);
	string threadfile = testpath + "/threads";
	Launcher l(&threadfile);

	BPatch_process *app = l.createProcess();
	REQUIRE(app != NULL);

	Instrumenter inst(app->getImage());
	REQUIRE(inst.getPCreate() != NULL);
	REQUIRE(inst.getFunction("pthread_exit") != NULL);

	SECTION("check library functions exist"){
		REQUIRE(inst.loadLibraries());
		BPatch_module *vex = inst.img->findModule("libvex.so", false);
		BPatch_module *cine = inst.img->findModule("libcine.so", false);
		REQUIRE(vex != NULL);
		REQUIRE(cine != NULL);

		//cine
		REQUIRE(inst.getFunction("cine_thread_create") != NULL);
		REQUIRE(inst.getFunction("orig_thread_create") != NULL);

		//vex
		REQUIRE(inst.getFunction("ThreadEventsBehaviour::onStart") != NULL);
		REQUIRE(inst.getFunction("ThreadEventsBehaviour::onEnd") != NULL);
	}

	//insert more here
	SECTION("control pthreads"){
		REQUIRE(inst.loadLibraries());
		//REQUIRE(inst.insertThreadCalls());
		//somehow check
		l.launch();
	}


}
