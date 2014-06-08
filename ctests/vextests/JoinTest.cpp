#include <ctime>
#include <iostream>
#include <math.h>
#include <string>
#include <unistd.h>
#include "pthread.h"

using namespace std;

	double temp = 1.0;
	bool testTimeOut = false;

  void *RunningThread(void *t){
      for (int i = 1; i<100000; i++) {
				temp += pow(1.0 + 1.0/i, i);				
			}
  }

  void *TestTimerThread(void *t){
    sleep(3);
    testTimeOut = true;
  }

	int main(int argc, char *argv[]) {
		int threads = stoi(argv[1]);
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t thread[threads];
    pthread_t timer;

    int round = 0;
		//long start = System.nanoTime();
    time_t start;
    time(&start);

    string timerText = "TestTimer";
    pthread_create(&timer, &attr, TestTimerThread, (void *)&timerText);

		while (!testTimeOut) {
      long i;
      for (i = 0; i < threads; i++) {
        pthread_create(&thread[i], &attr, RunningThread, (void *)i);
      }
      for (i = 0; i < threads; i++) {
        pthread_join(thread[i], NULL);
      }
			cout << "Round " << (++round) << " finished after " << (time(NULL)-start) << endl;
    }
  }
