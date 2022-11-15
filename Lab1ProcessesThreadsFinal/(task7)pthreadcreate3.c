#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct threadArgs {
	unsigned int id;
	unsigned int numThreads;
	unsigned int squaredId; //Create the new variable in the struct.
};

void* child(void* params) {
	struct threadArgs *args = (struct threadArgs*) params;
	unsigned int childID = args->id;
	unsigned int numThreads = args->numThreads;
	printf("Greetings from child #%u of %u\n", childID, numThreads);

	//Change the squareId variable to the squared value of the child thread ID. Then return the struct.
	args->squaredId = args->id * args->id;
	return (void*)args;
}

int main(int argc, char** argv) {
	pthread_t* children; // dynamic array of child threads
	struct threadArgs* args; // argument buffer
	unsigned int numThreads = 0;
	// get desired # of threads
	if (argc > 1)
		numThreads = atoi(argv[1]);
	children = malloc(numThreads * sizeof(pthread_t)); // allocate array of handles
	args = malloc(numThreads * sizeof(struct threadArgs)); // args vector
	for (unsigned int id = 0; id < numThreads; id++) {
		// create threads
		args[id].id = id;
		args[id].numThreads = numThreads;
		args[id].squaredId = -1; //Create starting value for squaredId to be -1. Anything squared can't be negative, so if -1 remains something went wrong.
		pthread_create(&(children[id]), // our handle for the child
			NULL, // attributes of the child
			child, // the function it should run
			(void*)&args[id]); // args to that function
	}
	printf("I am the parent (main) thread.\n");
	for (unsigned int id = 0; id < numThreads; id++) {
		//Change pthread_join to accept a return value as the second parameter.
		pthread_join(children[id], (void*)&args[id] );
	}

	//All child threads should be done, now print out the squared value for each thread.
	for (unsigned int id = 0; id < numThreads; id++) {
		printf("The squaredId value for child thread %d = %d\n", id, args[id].squaredId);
	}

	free(args); // deallocate args vector
	free(children); // deallocate array
	return 0;
}
