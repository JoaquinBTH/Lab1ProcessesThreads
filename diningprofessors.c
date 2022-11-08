#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> //Sleep

// Shared Variables
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
unsigned chopstickStatus[5] = {0, 0, 0, 0, 0};
struct professor
{
    unsigned int id;
    unsigned holdingLeft;
    unsigned holdingRight;
};

unsigned leftChopstick(struct professor *currentProf)
{
    //Enter critical section
    pthread_mutex_lock(&lock);

    unsigned succeeded = 0;

    if(chopstickStatus[currentProf->id] == 0) //Check if the left chopstick is taken.
    {
        chopstickStatus[currentProf->id] = 1;
        currentProf->holdingLeft = 1;

        printf("Professor %d picked up their left chopstick\n", currentProf->id);
        succeeded = 1;
    }
    else
    {
        printf("Professor %d failed at picking up their left chopstick\n", currentProf->id);
        succeeded = 0;
    }

    //Exit critical section
    pthread_mutex_unlock(&lock);

    return succeeded;
}

unsigned rightChopstick(struct professor *currentProf)
{
    //Enter critical section
    pthread_mutex_lock(&lock);

    unsigned succeeded = 0;

    if(chopstickStatus[(currentProf->id + 1) % 5] == 0) //Check if the right chopstick is taken.
    {
        chopstickStatus[(currentProf->id + 1) % 5] = 1;
        currentProf->holdingLeft = 1;

        printf("Professor %d picked up their right chopstick\n", currentProf->id);
        succeeded = 1;
    }
    else
    {
        printf("Professor %d failed at picking up their right chopstick\n", currentProf->id);
        succeeded = 0;
    }

    //Exit critical section
    pthread_mutex_unlock(&lock);

    return succeeded;
}

void putDownChopsticks(struct professor *currentProf)
{
    //Enter critical section
    pthread_mutex_lock(&lock);

    chopstickStatus[currentProf->id] = 0;
    chopstickStatus[(currentProf->id + 1) % 5] = 0;
    currentProf->holdingLeft = 0;
    currentProf->holdingRight = 0;

    printf("Professor %d put down their chopsticks\n", currentProf->id);

    //Exit critical section
    pthread_mutex_unlock(&lock);
}

void *cycle(void *args)
{
    struct professor *profs = (struct professor *)args;
    srand(time(NULL) * profs->id); // Initiate a random seed based on current time and id.

    /*
        1. Professors think between 1-5 seconds.
        2. Try to take the left chopstick.
        3. If they have the left chopstick, wait 2-8 seconds.
        4. Take the right chopstick.
        5. If they have both chopsticks, eat for 5-10 seconds.
        6. Put down chopsticks.
        7. Repeat.
    */

    int sleepTime = 0;

    // Start the cycle
    while (1)
    {
        // Enter first loop
        while (1)
        {
            // Think for 1-5 seconds.
            sleepTime = 1 + rand() % 5;
            printf("Professor %d is thinking for %d seconds...\n", profs->id, sleepTime);
            sleep(sleepTime);

            // Try to take the left chopstick.
            if(leftChopstick(profs))
                break;
        }

        //Enter second loop
        while(1)
        {
            //Think for 2-8 seconds.
            sleepTime = 2 + rand() % 7;
            printf("Professor %d is thinking again for %d seconds...\n", profs->id, sleepTime);
            sleep(sleepTime);

            // Try to take the right chopstick.
            if(rightChopstick(profs))
                break;
        }

        //Eat for 5-10 seconds.
        sleepTime = 5 + rand() % 6;
        printf("Professor %d is eating for %d seconds!!!\n", profs->id, sleepTime);
        sleep(sleepTime);

        //Put down both chopsticks
        putDownChopsticks(profs);
    }
}

int main()
{
    pthread_t profThreads[4];
    unsigned long nThreads = 5;

    struct professor professors[5];
    for (int i = 0; i < nThreads; i++)
    {
        professors[i].id = i;
        professors[i].holdingLeft = 0;
        professors[i].holdingRight = 0;
    }

    for (int i = 1; i < nThreads; i++)
    {
        pthread_create(&(profThreads[i - 1]), NULL, cycle, (void *)&professors[i]);
    }

    cycle((void *)&professors[0]); // Parent thread

    for (int i = 1; i < nThreads; i++)
    {
        pthread_join(profThreads[i - 1], NULL);
    }
    pthread_mutex_destroy(&lock);
    return 0;
}
