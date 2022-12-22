#include "mystack.h"
#include <stdlib.h>
#include <stdio.h>

node_t *top = NULL; // top is a pointer to the top node

void push(long *timeInMicroSeconds, int *interationCounter, int *cubic_is_0_reno_is_1) // push the time in micro seconds
{
    node_t *newN = malloc(sizeof(node_t)); // allocate memory for the new node
    newN->timeInMicroSeconds = timeInMicroSeconds;
    newN->interationCounter = interationCounter;
    newN->cubic_is_0_reno_is_1 = cubic_is_0_reno_is_1;
    newN->nextN = NULL; // make the new node point to NULL
    if (top == NULL)
    {
        top = newN; // if the stack is empty, make the new node the top node
    }
    if (top != NULL)
    {
        newN->nextN = top; // make the new node point to the old top node
        top = newN;
    }
}

long *pop() // function that returns the time in micro seconds
{
    if (first == NULL)
    { // if the stack is empty
        return NULL;
    }
    else // if the stack is not empty
    {
        return first; // return the first node
    }

    long *result = first->timeInMicroSeconds;

    // Print the algorithm name and iteration number
    if (*(first->cubic_is_0_reno_is_1) == 0)
    { // if the algorithm is cubic- print it to the user
        printf("algorithm is: cubic\n iter number: %d \n time: %ld micro_sec \n",
               *(first->interationCounter), *(first->timeInMicroSeconds));
    }
    if (*(first->cubic_is_0_reno_is_1) == 1) // if the algorithm is reno- print it to the user
    {
        printf("algorithm is: reno \n iter number: %d \n time: %ld micro_sec  \n",
               *(first->interationCounter), *(first->timeInMicroSeconds));
    }

    node_t *temp = first; // temp is a pointer to the first node

    first = first->nextN; // make the first node point to the next node

    // Free the memory used by the old first node
    free(temp);

    // Return the result
    return result; // return the time in micro seconds
}