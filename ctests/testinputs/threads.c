#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
 
#define NUM_THREADS     5
 
void *TaskCode(void *argument)
{
   int tid;
 
   tid = *((int *) argument);
   printf("Hello World! It's me, thread %d!\n", tid);
 
   /* optionally: insert more useful stuff here */
 
   return NULL;
}
 
int main(void)
{
   pthread_t threads[NUM_THREADS];
   int thread_args[NUM_THREADS];
   int rc, i, t;
 
   // create all threads one by one
   for (i=0; i<NUM_THREADS; ++i) {
      thread_args[i] = i;
      printf("In main: creating thread %d\n", i);
      rc = pthread_create(&threads[i], NULL, TaskCode, (void *) &thread_args[i]);
      assert(0 == rc);
   }
 
   // wait for each thread to complete
   for (i=0; i<NUM_THREADS; ++i) {
      // block until thread i completes
      rc = pthread_join(threads[i], NULL);
      printf("In main: thread %d is complete\n", i);
      assert(0 == rc);
   }
 
   printf("In main: All threads completed successfully\n", i);
   exit(EXIT_SUCCESS);
}
