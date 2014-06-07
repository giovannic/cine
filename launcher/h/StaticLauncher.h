/*
 * StaticLauncher.h
 *
 *  Created on: 19 May 2014
 *      Author: giovanni
 */

#ifndef STATICLAUNCHER_H_
#define STATICLAUNCHER_H_

#include "Launcher.h"

class StaticLauncher: public Launcher{
public:
	StaticLauncher();
	~StaticLauncher();
	bool launch();
private:
	bool setup();
	BPatch_binaryEdit *bin;
	BPatch_binaryEdit *openBinary();
};



#endif /* STATICLAUNCHER_H_ */
