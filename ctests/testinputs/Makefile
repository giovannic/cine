CFLAGS= -g
VEXDIR= /home/giovanni/vexjine/src/vex
CINELIB = /home/giovanni/cine/exp/lib
CINEINC = /home/giovanni/cine/profiler/collector/h

all: threads mutexthreads vexmutexthreads

clean:
	rm -f threads mutexthreads vexmutexthreads

threads: threads.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $< -o $@ -lpthread


mutexthreads: mutexthreads.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $< -o $@ -lpthread

vexmutexthreads: vexmutexthreads.cpp
	$(CXX) $(CXXFLAGS) -L$(CINELIB) -I$(CINEINC) $< -o $@ -lcine -lpthread 
