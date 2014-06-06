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

  /*
	public class RunningThread implements Runnable {

		@Override
		public void run() {
			for (int i = 1; i<100000; i++) {
				temp += Math.pow(1.0 + 1.0/i, i);				
			}
			
		}
		
	}
  */

  void *TestTimerThread(void *t){
    sleep(3);
    testTimeOut = true;
  }

/*
	public class TestTimerThread implements Runnable {

		@Override
		public void run() {
			try {
				Thread.sleep(7000);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			testTimeOut = true;
		}
		
	}
  */

	int main(int argc, char *argv[]) {
		int threads = stoi(argv[1]);
		//Thread[] thread = new Thread[threads];
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t thread[threads];
    pthread_t timer;

    int round = 0;
		//long start = System.nanoTime();
    time_t start;
    time(&start);

		//Thread timer = new Thread(j.new TestTimerThread(), "TestTimer");
		//timer.start();
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
		
    /*
		while (!testTimeOut) {
			for (int i = 0; i<threads; i++) {
				thread[i] = new Thread(j.new RunningThread(), "RunningThread" + i);
				thread[i].start();
			}
			for (int i = 0; i<threads; i++) {
				try {
					thread[i].join();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}		
		}
    */
