/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h> //Include threads

#define SIZE 1024

//Define struct for typecasting
struct threadRow
{
    unsigned int row;
};

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];

static void
init_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++)
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

void *matmul_seq(void *row)
{
    struct threadRow* i = (struct threadRow*) row;

    int j, k;

    for (j = 0; j < SIZE; j++)
    {
        c[i->row][j] = 0.0;
        for (k = 0; k < SIZE; k++)
            c[i->row][j] = c[i->row][j] + a[i->row][k] * b[k][j];
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
    init_matrix();

    // Initialize all the 1024 threads.
    pthread_t thread[SIZE];
    struct threadRow rows[SIZE];

    for (int i = 0; i < SIZE; i++)
    {
        rows[i].row = i;
        pthread_create(&thread[i], NULL, matmul_seq, (void*)&rows[i]);
    }

    for(int i = 0; i < SIZE; i++)
    {
        pthread_join(thread[i], NULL);
    }
    // print_matrix();

    return 0;
}
