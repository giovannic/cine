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
		vector<string *>args;
		args.push_back(new string ("hello world"));

		DynamicLauncher l(echo, &args);

		REQUIRE(l.launch()); //no errors

	}

	SECTION("dynamic "){
		//basic
		string testpath(BIN_PATH);
		string threadfile = testpath + "/threads";
		vector<string *>args;
		DynamicLauncher l(threadfile, &args);

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
