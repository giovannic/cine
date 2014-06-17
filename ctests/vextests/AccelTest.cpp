#include <ctime>
#include <iostream>
#include <math.h>
#include <string>
#include <unistd.h>
#include "pthread.h"

using namespace std;

  long load;
	double temp = 1.0;
	bool testTimeOut = false;

  void *RunningThread(void *t){
    for (int j = 0; j < 200; j++){
      for (int i = 1; i<load; i++) {
				temp += pow(1.0 + 1.0/i, i);				
			}
      temp = 0;
    }
  }

	int main(int argc, char *argv[]) {
		int threads = stoi(argv[1]);
		load = stol(argv[2]);
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t thread[threads];
    pthread_t timer;

    int round = 0;
    time_t start;
    time(&start);

      long i;
      for (i = 0; i < threads; i++) {
        pthread_create(&thread[i], &attr, RunningThread, (void *)i);
      }
      for (i = 0; i < threads; i++) {
        pthread_join(thread[i], NULL);
      }
			cout << "Round " << (++round) << " finished after " << (time(NULL)-start) << endl;
  }
