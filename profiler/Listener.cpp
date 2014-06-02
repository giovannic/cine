/*
 * Listener.cpp
 *
 *  Created on: 2 Jun 2014
 *      Author: giovanni
 */

#include <Listener.h>

#include <iostream>

#include <sys/stat.h>
#include <sys/errno.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

Listener::Listener() {
	// TODO Auto-generated constructor stub
	listener = 0;
}

Listener::~Listener() {
	pthread_cancel(listener);
	if(invFifo > 0){
		close(invFifo);
	}
}

void *doListen(void *pfifo){
	char buf[5];//just method ids or continue
	const char *pipefile = "/tmp/cineinv";
	int fifo = *(int *)pfifo;

	//TODO:permissions
	if (mkfifo(pipefile, 0666) < 0 && errno != EEXIST){
		perror("pipe failed");
		exit(1);
	}

	fifo = open(pipefile, O_RDWR);
	if (fifo < 0){
		perror("cannot open pipe");
		exit(1);
	}
	while(true){
		if(read(fifo, buf, sizeof(buf)) >= 0){
			cout << "about to invalidate " << buf << endl;
		}
	}
	return NULL;
}

void Listener::listen() {
//	listenpid = fork(); //could be threaded?
//	if (listenpid == -1){
//		perror("fork failed");
//	}
//	if (listenpid > 0){
//		return;
//	}
	pthread_create(&listener, NULL, doListen, &invFifo);
}
