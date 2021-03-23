#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

int numDivMax = 0; // most # of divisor
int n = 1;         // size of dynamic array in case the result is more than one value
int *result;       // value with the most divisor

struct Range
{
    int lower; // lower bound
    int upper; // upper bound
};

void *runner(void *param); // threads call this function

int main(int argc, char *argv[])
{
    int x;
    result = (int *)malloc(1 * sizeof(int)); // dynamic array of size 1

    printf("\nEnter the number of threads: ");
    scanf("%d", &x);

    struct timeval tval_before, tval_after, tval_result;
    gettimeofday(&tval_before, NULL); // timer starts

    struct Range ranges[x]; // creating array of ranges for multiple threads based on number of threads

    for (int i = 0; i < x; i++) // initializing the array of ranges
    {
        ranges[i].lower = i * 10000 / x + 1;
        ranges[i].upper = (i + 1) * 10000 / x;
    }

    pthread_t tids[x];   // the thread identifier
    pthread_attr_t attr; // set of thread attributes

    pthread_attr_init(&attr); // get the default attributes

    for (int i = 0; i < x; i++) // create x(user input) threads
    {
        pthread_create(&tids[i], &attr, runner, &ranges[i]);
    }

    for (int i = 0; i < x; i++) // join x(user) threads
        pthread_join(tids[i], NULL);

    gettimeofday(&tval_after, NULL); // timer ends

    timersub(&tval_after, &tval_before, &tval_result);

    printf("number: ");
    for (int i = 0; i < n; i++) // for loop in case the result is more than one value
    {
        printf("%d", result[i]);
        if (i + 1 == n)
            break;
        printf(", ");
    }
    printf("\nfactors: %d\ntime: %ld.%06ld\n", numDivMax, (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);

    return 0;
}

void *runner(void *param)
{
    struct Range *param_range = (struct Range *)param;

    for (int i = param_range->lower; i <= param_range->upper; i++)
    {
        int temp = 0;
        for (int j = 1; j <= i; j++)
            if (i % j == 0) // checking if j divides i
                temp += 1;
        if (temp > numDivMax) // update most # of divisor and value with the most divisor
        {
            n = 1;
            result = (int *)realloc(result, n * sizeof(int)); // reallocate the memory to size 1 in case the previous result was more than one
            numDivMax = temp;
            result[n - 1] = i;
        }
        else if (temp == numDivMax)
        {
            n += 1;
            result = (int *)realloc(result, n * sizeof(int)); // increase the size of dynamic array by 1
            result[n - 1] = i;
        }
    }

    pthread_exit(0); // terminates the thread
}