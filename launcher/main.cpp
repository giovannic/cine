/*
 * main.c
 *
 *  Created on: 6 Feb 2014
 *      Author: giovanni
 */

#include"StaticLauncher.h"
#include"DynamicLauncher.h"
#include <stdlib.h>

int main(int argc, char* argv[]) {
	//TODO: validate and add program arguments
	string input(argv[1]);
	vector<string *> args;

	if(argc > 1){
		string *arg;
		for(int a = 1; a < argc; a++){
			arg = new string(argv[a]);
			args.push_back(arg);
		}
	}
	DynamicLauncher l(input, &args);

	l.setup();
	l.launch();

	exit(0);
}
