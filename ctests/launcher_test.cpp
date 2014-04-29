/*
 * launcher_test.c
 *
 *  Created on: 5 Feb 2014
 *      Author: giovanni
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Launcher.h"
#include <unistd.h>
#include <iostream>
#include <sstream>

using namespace std;

TEST_CASE( "Create process", "[loader]" ) {

	SECTION("multithreaded"){
		//basic
		string testpath(BIN_PATH);
		string threadfile = testpath + "/threads";
		Launcher l(&threadfile);

		BPatch_process *app = l.createProcess();
		REQUIRE(app != NULL);
		l.launch(); //no errors
	}

	//some kind of thread problem
	SECTION("with arguments"){

		string echo = "/bin/echo";
		string args = "hello world";

		Launcher l(&echo);
		l.add_arguments(&args);

		BPatch_process *app = l.createProcess();
		REQUIRE(app != NULL);
		l.launch(); //no errors

	}

	//multiple arguments
}

//
//TEST_CASE( "Arguments", "[loader]" ) {
//
//	//arguments

//
//}
