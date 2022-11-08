#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h> /* For O_* constants */

//Includes from shmem
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h> //Include time
#include <sys/wait.h> //Include wait

//Defines from shmem
#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200

const char *semName1 = "my_sema1";
const char *semName2 = "my_sema2";

int main(int argc, char **argv)
{
	//Parts from Shmem
	srand(time(NULL)); //Start a random seed.
	struct shm_struct {
		int buffer[10]; //Change the buffer size to an array of 10 elements.
	};
	volatile struct shm_struct *shmp = NULL;
	char *addr = NULL;
	int var1 = 0, var2 = 0, shmid = -1;
	struct shmid_ds *shm_buf;

	float waitTime; //Create a float variable that will act as the target time to wait.

	/* allocate a chunk of shared memory */
	shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
	shmp = (struct shm_struct *) shmat(shmid, addr, 0);


	pid_t pid;
	sem_t *sem_id1 = sem_open(semName1, O_CREAT, O_RDWR, 1);
	sem_t *sem_id2 = sem_open(semName2, O_CREAT, O_RDWR, 0);
	int i, status;

	pid = fork();
	if (pid) {
		for (i = 0; i < 100; i++) {
			sem_wait(sem_id1);
			//Add code from shmem
			var1++;
			printf("Sending %d\n", var1); fflush(stdout);
			shmp->buffer[((var1 - 1) % 10)] = var1; //Add the number to the correct position in the buffer, between 0-9.

			waitTime = ((float)rand()/(float)RAND_MAX) * 0.5f; //Selects a value between 0-1 and then multiply it by 0.5f to get it as the maximum value possible.
			if(waitTime < 0.1f)
			{
				waitTime = 0.1f; //If the time was lower than 0.1 seconds, set the waitTime to minimum.
			}

			usleep(waitTime * 1000000); //Convert waitTime into microseconds and sleep for that amount of time.

			sem_post(sem_id2);
		}
		//Add code from shmem
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);

		sem_close(sem_id1);
		sem_close(sem_id2);
		wait(&status);
		sem_unlink(semName1);
		sem_unlink(semName2);
	} else {
		for (i = 0; i < 100; i++) {
			sem_wait(sem_id2);

			//Add code from shmem
			var2 = shmp->buffer[(var2 % 10)]; //Get the value from the correct spot in the buffer.
			printf("Received %d\n", var2); fflush(stdout); //Moved the printing to before setting the empty value to 0.
			
			waitTime = ((float)rand()/(float)RAND_MAX) * 2.0f; //Selects a value between 0-1 and then multiply it by 2.0f to get it as the maximum value possible.
			if(waitTime < 0.2f)
			{
				waitTime = 0.2f; //If the time was lower than 0.2 seconds, set the waitTime to minimum.
			}

			usleep(waitTime * 1000000); //Convert waitTime into microseconds and sleep for that amount of time.

			sem_post(sem_id1);
		}
		//Code from shmem
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);

		sem_close(sem_id1);
		sem_close(sem_id2);
	}
}
