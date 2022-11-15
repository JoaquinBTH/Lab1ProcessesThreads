#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <time.h> //Include time
#include <stdlib.h> //Include random
#include <sys/wait.h> //Include wait

#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200

int main(int argc, char **argv)
{
	srand(time(NULL)); //Start a random seed.
	struct shm_struct {
		int buffer[10]; //Change the buffer size to an array of 10 elements.
		int startPos; //Current start position of the circular buffer
		int endPos; //Current end position of the circular buffer
	};
	volatile struct shm_struct *shmp = NULL;
	char *addr = NULL;
	pid_t pid = -1;
	int var1 = 0, var2 = 0, shmid = -1;
	struct shmid_ds *shm_buf;

	float waitTime; //Create a float variable that will act as the target time to wait.

	/* allocate a chunk of shared memory */
	shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
	shmp = (struct shm_struct *) shmat(shmid, addr, 0);
	shmp->startPos = 0;
	shmp->endPos = 0;
	pid = fork();
	if (pid != 0) {
		/* here's the parent, acting as producer */
		while (var1 < 100) {
			/* write to shmem */
			var1++;
			//Without this while loop, the problem of adding new numbers before the consumer has fetched the number would occur. Causing us to lose numbers
			while (shmp->endPos - shmp->startPos > 9); /* busy wait until the buffer isn't full */
			printf("Sending %d\n", var1); fflush(stdout);
			shmp->buffer[((var1 - 1) % 10)] = var1; //Add the number to the correct position in the buffer, between 0-9.
			shmp->endPos++;

			waitTime = ((float)rand()/(float)RAND_MAX) * 0.5f; //Selects a value between 0-1 and then multiply it by 0.5f to get it as the maximum value possible.
			if(waitTime < 0.1f)
			{
				waitTime = 0.1f; //If the time was lower than 0.1 seconds, set the waitTime to minimum.
			}

			usleep(waitTime * 1000000); //Convert waitTime into microseconds and sleep for that amount of time.
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	} else {
		/* here's the child, acting as consumer */
		while (var2 < 100) {
			/* read from shmem */
			while (shmp->endPos - shmp->startPos == 0); /* busy wait until there is something */
			//If the endPos and startPos would decide what value to be picked it could cause the same numbers to be repeated if for example both variables
			//were to increment at the same time when fetching the value. Since I'm using it based on the previous var2 this will never occur. But losing numbers can still occur.
			var2 = shmp->buffer[(var2 % 10)]; //Get the value from the correct spot in the buffer.
			printf("Received %d\n", var2); fflush(stdout); //Moved the printing to before setting the empty value to 0.
			shmp->startPos++;

			waitTime = ((float)rand()/(float)RAND_MAX) * 2.0f; //Selects a value between 0-1 and then multiply it by 2.0f to get it as the maximum value possible.
			if(waitTime < 0.2f)
			{
				waitTime = 0.2f; //If the time was lower than 0.2 seconds, set the waitTime to minimum.
			}

			usleep(waitTime * 1000000); //Convert waitTime into microseconds and sleep for that amount of time.
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	}

	if(pid != 0)
	{
		//Parent process
		pid = wait(NULL); //Prevent terminal from thinking the program is finished because the parent process finished too early.
	}
}
