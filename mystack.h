#ifndef MYSTACK
#define MYSTACK

struct first
{
    struct first *nextN;
    long int *timeInMicroSeconds;
    int *interationCounter;
    int *cubic_is_0_reno_is_1;
};

typedef struct first node_t;
void push(long *timeInMicroSeconds, int *interationCounter, int *cubic_is_0_reno_is_1);
long *pop();

extern node_t *first;

#endif // MYSTACK