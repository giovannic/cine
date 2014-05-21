/*
 * launcher_test.c
 *
 *  Created on: 5 Feb 2014
 *      Author: giovanni
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "StaticLauncher.h"
#include "DynamicLauncher.h"
#include <unistd.h>
#include <iostream>
#include <sstream>

using namespace std;

TEST_CASE( "Create process", "[loader]" ) {

	SECTION("multithreaded"){
		//basic
		string testpath(BIN_PATH);
		string threadfile = testpath + "/threads";
		StaticLauncher l(threadfile);

		REQUIRE(l.launch()); //no errors
	}

	//some kind of thread problem
	SECTION("with arguments"){

		string echo("/bin/echo");
		string args("hello world");

		StaticLauncher l(echo);
		l.add_arguments(args);

		REQUIRE(l.launch()); //no errors

	}

	SECTION("dynamic "){
		//basic
		string testpath(BIN_PATH);
		string threadfile = testpath + "/threads";
		DynamicLauncher l(threadfile);

		REQUIRE(l.launch()); //no errors
	}
	//multiple arguments
}

//
//TEST_CASE( "Arguments", "[loader]" ) {
//
//	//arguments

//
//}
