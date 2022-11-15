#include <stdio.h>
#include <unistd.h>

#include <sys/wait.h>
/*
Sys wait is not required for the program to work, it simply helps in removing any terminal 
issues that can occur when the parent process finishes before the child processes are done.
*/

int main(int argc, char **argv)
{
    pid_t pid, pidTwo; // Create a pid_t value for each child process.
    unsigned i;
    unsigned niterations = 100;
    pid = fork(); //First child process
    if (pid == -1)
    {
        printf("Error creating first child\n");
    }
    else if (pid == 0)
    {
        for (i = 0; i < niterations; ++i)
            printf("A = %d, ", i);
    }
    else //Parent process
    {
        pidTwo = fork(); //Second child process
        if (pidTwo == -1)
        {
            printf("Error creating second child\n");
        }
        else if (pidTwo == 0)
        {
            for (i = 0; i < niterations; ++i)
                printf("C = %d, ", i);
        }
        else //Parent process
        {
            for (i = 0; i < niterations; ++i)
                printf("B = %d, ", i);
        }
    }

    if (pid > 0 && pidTwo > 0)
    {
        // Parent process
        pid = wait(NULL); //Waits for the first child to finish its process (not required)
        pidTwo = wait(NULL); //Waits for the second child to finish its process (not required)
        printf("\nID of Child 1: %d\nID of Child 2: %d", pid, pidTwo);
    }

    printf("\n");
}
