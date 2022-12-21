#ifndef MYSTACK
#define MYSTACK

struct first // node
{
    struct first *nextN; // pointer to next node
    long int *timeInMicroSeconds;
    int *interationCounter;
    int *cubic_is_0_reno_is_1; // 0 for cubic, 1 for reno
};

typedef struct first node_t;
void push(long *timeInMicroSeconds, int *interationCounter, int *cubic_is_0_reno_is_1);

long *pop();

extern node_t *first;

#endif // MYSTACK