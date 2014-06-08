CXXFLAGS= -g -std=c++11
VEXDIR= /home/giovanni/vexjine/src/vex
CINELIB = /home/giovanni/cine/lib
CINEINC = /home/giovanni/cine/profiler/h

all: JoinTest.o TimersTest.o

clean:
	rm -f JoinTest.o TimersTest.o

JoinTest.o: JoinTest.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ -lpthread 

TimersTest.o: TimersTest.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
