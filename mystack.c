#include "mystack.h"
#include <stdlib.h>
#include <stdio.h>

node_t *first = NULL;

void push(long *timeInMicroSeconds, int *interationCounter, int *cubic_is_0_reno_is_1)
{
    node_t *newN = malloc(sizeof(node_t));
    newN->timeInMicroSeconds = timeInMicroSeconds;
    newN->interationCounter = interationCounter;
    newN->cubic_is_0_reno_is_1 = cubic_is_0_reno_is_1;
    newN->nextN = NULL;
    if (first == NULL)
    {
        first = newN;
    }
    else
    {
        newN->nextN = first;
        first = newN;
    }
}

long *pop() {
    if (first == NULL) {
        return NULL;
    }

    long *result = first->timeInMicroSeconds;

    // Print the algorithm name and iteration number
    if (*(first->cubic_is_0_reno_is_1) == 0) {
        printf("algorithm is: cubic iter number: %d time: %ld micro_sec \n",
        *(first->interationCounter), *(first->timeInMicroSeconds));
    } else if (*(first->cubic_is_0_reno_is_1) == 1) {
        printf("algorithm is: reno iter number: %d time: %ld micro_sec  \n",
        *(first->interationCounter), *(first->timeInMicroSeconds));
    }

    // Save a pointer to the current first node
    node_t *temp = first;

    // Set the first node to be the next node in the list
    first = first->nextN;

    // Free the memory used by the old first node
    free(temp);

    // Return the result
    return result;
}