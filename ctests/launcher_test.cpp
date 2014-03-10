/*
 * launcher_test.c
 *
 *  Created on: 5 Feb 2014
 *      Author: giovanni
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "launcher.hpp"
#include "OProfile.hpp"
#include <unistd.h>
#include <iostream>
#include <sstream>

using namespace std;

TEST_CASE( "Loader", "[loader]" ) {

	//basic
	string testpath(BIN_PATH);
	string threadfile = testpath + "/threads";
	REQUIRE( Launcher(&threadfile).launch() );

	//profiler
	//l = Launcher(&threadfile);
	//string opdir = "/usr/local/bin";
	//REQUIRE( l.launch() );
}

TEST_CASE( "Arguments", "[loader]" ) {

	//arguments
	string echo = "/bin/echo";
	string args = "hello world";

	Launcher l = Launcher(&echo);
	l.add_arguments(&args);

	REQUIRE( l.launch() );
}

TEST_CASE( "Vex integration", "[loader]" ) {


}
