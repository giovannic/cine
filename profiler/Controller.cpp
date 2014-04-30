/*
 * Controller.cpp
 *
 *  Created on: 29 Apr 2014
 *      Author: giovanni
 */

#include "Controller.h"

#include "callbacks.h"

#include "PCProcess.h"
#include "Event.h"

#include "VTF.h"

using namespace Dyninst::ProcControlAPI;
using namespace VEX;

void Controller::listenThreads(){

//	Tell ProcControlAPI about our callback functions
//	Process::registerEventCallback(EventType::UserThreadCreate, on_thread_create);
//	Process::registerEventCallback(EventType::UserThreadDestroy, on_thread_exit);

	Process::registerEventCallback(EventType::LWPCreate, on_thread_create);
	Process::registerEventCallback(EventType::LWPDestroy, on_thread_exit);
}

Controller::Controller(Analyser *a, BPatch_process *proc){
	cb_create_function = a->getFunction("cine_on_create");
	cb_exit_function = a->getFunction("cine_on_exit");
	cb_main_proc = proc;
}

Controller::~Controller() {
}

