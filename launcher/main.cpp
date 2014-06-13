/*
 * main.c
 *
 *  Created on: 6 Feb 2014
 *      Author: giovanni
 */

#include"StaticLauncher.h"
#include"DynamicLauncher.h"
#include <stdlib.h>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
	//TODO: validate and add program arguments
	list<string> args(argv + 1, argv + argc);
	Launcher *l;
	string *input;
	vector<string> *inputArgs;

	while(!args.empty()){
		string token = args.front();
		if(token[0] == '-'){ //vexargs
			args.pop_front();

			if(token == string("-static")){
				l = new StaticLauncher();
			} else if(token == string("-dynamic")){
				l = new DynamicLauncher();
			} else if(token == string("-speedup")){
				l->registerSpeedup(args.front(), stod(*(args.begin()++)));
				args.pop_front(); args.pop_front();
			} else if(token == string("-adapt")){
				l->registerInvalidation(args.front());
				args.pop_front();
			} else if(token == string("-methodfile")){
				l->registerMethodFile(args.front());
				args.pop_front();
			}

		} else {
			input = new string(token);
			l->setInput(*input);
			inputArgs = new vector<string>(args.begin(), args.end());
			l->setArgs(*inputArgs);
			args.clear();
		}
	}

	l->setup();
	l->launch();

	exit(0);
}


