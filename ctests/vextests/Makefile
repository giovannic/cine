CXXFLAGS= -g -std=c++11
VEXDIR= /home/giovanni/vexjine/src/vex
CINELIB = /home/giovanni/cine/lib
CINEINC = /home/giovanni/cine/profiler/h

all: JoinTest.o

clean:
	rm -f JoinTest.o

JoinTest.o: JoinTest.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ -lpthread 
