/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h> //Include threads

#define SIZE 1024

// Define struct for typecasting
struct threadID
{
    unsigned int id;
};

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];

void *init_matrix(void *idArg)
{
    struct threadID *idVal = (struct threadID *)idArg;

    int i, j;

    int maxVal = (idVal->id + 1) * 341;

    if(maxVal == 1023)
    {
        maxVal = 1024;
    }

    for (i = idVal->id * 341; i < maxVal; i++)
    {
        for (j = 0; j < SIZE; j++)
        {
            /* Simple initialization, which enables us to easy check
             * the correct answer. Each element in c will have the same
             * value as SIZE after the matmul operation.
             */
            a[i][j] = 1.0;
            b[i][j] = 1.0;
        }
    }
}

void *matmul_seq(void *idArg)
{
    struct threadID *idVal = (struct threadID *)idArg;

    int i, j, k;

    int maxVal = (idVal->id + 1) * 341;

    if(maxVal == 1023)
    {
        maxVal = 1024;
    }

    for (i = idVal->id * 341; i < maxVal; i++)
    {
        for (j = 0; j < SIZE; j++)
        {
            c[i][j] = 0.0;
            for (k = 0; k < SIZE; k++)
                c[i][j] = c[i][j] + a[i][k] * b[k][j];
        }
    }
}

static void
print_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE; j++)
            printf(" %7.2f", c[i][j]);
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    // Initialize 3 threads.
    pthread_t thread[3];
    struct threadID ids[3];

    // Initialize matrices
    for (int i = 0; i < 3; i++)
    {
        ids[i].id = i;
        pthread_create(&thread[i], NULL, init_matrix, (void *)&ids[i]);
    }

    // Join all threads
    for (int i = 0; i < 3; i++)
    {
        pthread_join(thread[i], NULL);
    }

    // Multiply matrices
    for (int i = 0; i < 3; i++)
    {
        pthread_create(&thread[i], NULL, matmul_seq, (void *)&ids[i]);
    }

    // Join all threads
    for (int i = 0; i < 3; i++)
    {
        pthread_join(thread[i], NULL);
    }
    //print_matrix();

    return 0;
}
