#include "mystack.h"
#include <stdlib.h>
#include <stdio.h>

node_t *first = NULL;

void push(long *timeInMicroSeconds, int *interationCounter, int *cubic_is_0_reno_is_1)
{
    node_t *new_node = malloc(sizeof(node_t));
    if (new_node == NULL)
    {
        // Handle the case where malloc fails to allocate memory
        return;
    }

    new_node->timeInMicroSeconds = timeInMicroSeconds;
    new_node->interationCounter = interationCounter;
    new_node->cubic_is_0_reno_is_1 = cubic_is_0_reno_is_1;
    new_node->nextN = first;
    first = new_node;
}

long *pop()
{
    if (first == NULL)
    {
        return NULL;
    }

    long *result = first->timeInMicroSeconds;

    // Print the algorithm name and iteration number
    switch (*(first->cubic_is_0_reno_is_1))
    {
    case 0:
        printf("algorithm is: cubic iter number: %d time: %ld micro_sec \n",
               *(first->interationCounter), *(first->timeInMicroSeconds));
        break;
    case 1:
        printf("algorithm is: reno iter number: %d time: %ld micro_sec \n",
               *(first->interationCounter), *(first->timeInMicroSeconds));
        break;
    default:
        // Handle the case where the value of *(first->cubic_is_0_reno_is_1) is not 0 or 1
        break;
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