CFLAGS= -g

all: threads mutexthreads

clean:
	rm -f threads mutexthreads

threads: threads.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $< -o $@ -lpthread


mutexthreads: mutexthreads.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $< -o $@ -lpthread
