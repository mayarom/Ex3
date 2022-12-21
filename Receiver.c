#include <sys/time.h>
#include <netinet/in.h>
#include <limits.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "mystack.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAYA 5251          // first id
#define YOGEV 9881         // second id
#define ERROR (-1)         // error code
#define fileSize 1462544   // file size
#define receiverPORT 5030  // port number
#define BUFFERSIZE 1462544 // buffer size
#define BACKLOG 100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

char message[BUFFERSIZE];
void check_conn(int senderSocket, int receiverSocket);
int sendback(int number, int fd);
void print_the_status(int number_of_iterations);

int main(int argc, char **argv)
{
    int receiverSocket, senderSocket, addrSize;
    SA_IN receiver_addr, sender_addr;

    if ((receiverSocket = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
    {
        perror("sorry, socket failed");
        exit(1);
    }

    // initialize the adress struct
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_addr.s_addr = INADDR_ANY;
    receiver_addr.sin_port = htons(receiverPORT);

    // bind the socket to the port
    switch (1)
    {
    case 1:
        if (bind(receiverSocket, (SA *)&receiver_addr, sizeof(receiver_addr)) == ERROR) // the cast is needed to avoid a warning
        {
            perror("sorry, bind failed, you should try again");
            exit(1);
        }
        break;
    case 2:
        if (listen(receiverSocket, BACKLOG) == ERROR) // the cast is needed to avoid a warning
        {
            perror("sorry, listen failed, you should try again");
            exit(1);
        }
        break;
    default:
        break;
    }

    while (1)
    {
        printf("the socket is waiting for connections... \n");
        // wait for and eventually accept an incomming connection

        addrSize = sizeof(SA_IN);
        if ((senderSocket = accept(receiverSocket, (SA *)&sender_addr, (socklen_t *)&addrSize)) == ERROR) // the cast is needed to avoid a warning
        {
            perror("sorry, accept failed, you should try again");

            printf("socket has been closed, exiting the program\n");

            exit(1);
        }
        printf(" good luck connected succesfully!\n");

        check_conn(senderSocket, receiverSocket); // the function that will run when we get a connection
    }
    close(receiverSocket); // close the socket
    printf("closing the receiver socket\n");
}

void check_conn(int senderSocket, int receiverSocket) // the function that will run when we get a connection
{
    // recieve the first part of the file

    uint32_t XOR = MAYA ^ YOGEV; // the XOR of the two ports
    int interationCounter = 0;   // the number of iterations
    int numOfBytes = 0;          // the number of bytes we have received

    while (1) // the loop that will run until we get the whole file
    {
        struct timeval start_t_cubic, end_t_cubic, timeResultCubic;
        struct timeval startReno, endReno, timeResultReno;

        interationCounter++;        // increase the number of iterations
        bzero(message, BUFFERSIZE); // clear the buffer

        // set the algorithm to cubic
        char *cc = "cubic";
        if (setsockopt(senderSocket, IPPROTO_TCP, TCP_CONGESTION, cc, strlen(cc)) > 0 || setsockopt(senderSocket, IPPROTO_TCP, TCP_CONGESTION, cc, strlen(cc)) < 0)
        {
            printf("setsockopt has been failed \n"); // if the setsockopt has been failed
            return;                                  // exit the program
        }
        printf("** we change the cc algorithem to %s\n** ", cc); // print the algorithm

        gettimeofday(&start_t_cubic, NULL); // starts the time

        for (; numOfBytes < BUFFERSIZE / 2; numOfBytes++) //]
        {
            if (recv(senderSocket, message, 1, 0) == -1)
            {
                printf("so sad! receive has been failed. you should try again \n");
                break;
            }
        }
        usleep(1000);
        gettimeofday(&end_t_cubic, NULL); // finish count for first part of the file

        bzero(message, BUFFERSIZE);                               // clear the buffer
        timersub(&end_t_cubic, &start_t_cubic, &timeResultCubic); // the total time cubic

        long int *timeElapseCubic = (long int *)malloc(sizeof(long int)); // the time in micro seconds
        *timeElapseCubic = timeResultCubic.tv_sec * 1000000 + timeResultCubic.tv_usec;
        int *iteration_num_pointer = (int *)malloc(sizeof(int)); // the number of iterations
        *iteration_num_pointer = interationCounter;              // the number of iterations
        int *cubicParam = (int *)malloc(sizeof(int));
        *cubicParam = 0;
        push(timeElapseCubic, iteration_num_pointer, cubicParam); // push the time and the number of iterations to the stack

        printf("algo: cubic, time: %ld.%06ld,the number of the times we sent it: %d\n",
               (long int)timeResultCubic.tv_sec, (long int)timeResultCubic.tv_usec, interationCounter);

        // 6. Send back the authentication to the sender.
        // send thhe XOR to the sender
        printf("sending the XOR to the sender %d \n", XOR);
        sendback(XOR, senderSocket);

        bzero(message, BUFFERSIZE);

        // change the algorithm to reno
        char *ccAlgorithm = "reno"; // the CC algorithm to use (in this case, "reno")
        if (setsockopt(receiverSocket, IPPROTO_TCP, TCP_CONGESTION, ccAlgorithm, strlen(ccAlgorithm)))
        {
            perror("setsockopt failed");
            exit(1);
        }
        printf("** we change the CC algorithem to %s\n ***", ccAlgorithm);

        // recive the second part of the file
        gettimeofday(&startReno, NULL); // start the time

        // recive a file of half mega bytes
        for (size_t i = 0; i < BUFFERSIZE; i++)
        {
            if (recv(senderSocket, message, 1, 0) == -1)
            {
                printf("sorry, the recive is failed \n");
                break;
            }
        }

        usleep(1000); // sleep for 1 second

        gettimeofday(&endReno, NULL);                    // finish count for first part of the file
        timersub(&endReno, &startReno, &timeResultReno); // the total time reno
        printf("algo: reno, time: %ld.%06ld, iter number: %d\n", (long int)timeResultReno.tv_sec, (long int)timeResultReno.tv_usec, interationCounter);
        // store the time elapsed in a variable
        long int timeElapseReno = timeResultReno.tv_sec * 1000000 + timeResultReno.tv_usec;
        long int *timeElapseRenoP = (long int *)malloc(sizeof(long int));
        *timeElapseRenoP = timeElapseReno;
        int *renoParam = (int *)malloc(sizeof(int));
        *renoParam = 1; // the reno parameter
        push(timeElapseRenoP, iteration_num_pointer, renoParam);

        // if you get the exit message from the sender, close the socket and exit the program
        recv(senderSocket, message, 1024, 0); // recive the message from the sender
        // if the sender send the message "again" the receiver will recive the file again and send the report
        if (strcmp(message, "again") == 0) // if the sender send the message "again" the receiver will recive the file again and send the report
        {
            continue;
        }
        else
        {
            // print out the report
            printf("\n\n");
            printf("the Execution time report \n");
            print_the_status(interationCounter);
            close(senderSocket);   // close the sender socket
            close(receiverSocket); // close the receiver socket

            printf("good bye! closing the sender socket \n");
            printf("closing the receiver socket \n");
            printf("good bye! have a good day \n");
            printf("-maya and yogev- \n");

            return;
        }
    }
}

int sendback(int number, int fd)
{
    int32_t conv = htonl(number); // convert the number to network byte order
    char *data = (char *)&conv;   // the number to send
    int total_sent = 0;           // the total number of bytes we sent
    int sent;                     // the number of bytes we sent

    while (total_sent < sizeof(conv)) // while we didnt send all the bytes
    {
        sent = write(fd, data + total_sent, sizeof(conv) - total_sent); // send the number
        while (sent < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                printf(" sorry, there is a problem with sending");
                return -1;
            }
            else if (errno != EINTR)
            {
                printf(" sorry, there is a problem with sending");
                return -1;
            }
        }

        if (sent == 0) // we didnt send anything
        {
            printf("we didnt send anything"); // we didnt send anything
            return -1;
        }
        else // we sent something
        {
            total_sent = total_sent + sent; // update the total number of bytes we sent
        }
    }

    return 0; // we sent the number
}

void print_the_status(int number_of_iterations) // print the status of the program
{
    // first = the first node in the stack
    long int averageC = 0;     // the average time for cubic
    long int delete_num = 0;   // the number of nodes that we delete from the stack
    long int averageR = 0;     // the average time for reno
    long int averageTotal = 0; // the average time for total

    // pop the stack and print out the report
    while (first != NULL) // while the stack is not empty
    {
        if (*first->cubic_is_0_reno_is_1 == 0) // if the algorithm is cubic
        {
            averageC = averageC + *first->timeInMicroSeconds; // add the time to the average
        }
        else if (*first->cubic_is_0_reno_is_1 == 1) // if the algorithm is reno
        {
            averageR += *first->timeInMicroSeconds; // add the time to the average
        }
        averageTotal += *first->timeInMicroSeconds;

        printf("algo: %s, time: %ld, iter number: %d\n", *first->cubic_is_0_reno_is_1 == 0 ? "cubic" : "reno",
               *first->timeInMicroSeconds, *first->interationCounter);

        pop(); // delete the node from the stack

        delete_num = delete_num + 1; // count the number of nodes that we delete from the stack
    }
    // print out the report

    printf(" \n"); // print out the report
    printf("*** The results of the time measurements: ***  \n");

    printf("The average time for cubic is %ld microseconds, for reno is %ld microseconds,and for total is %ld microseconds\n", averageC / number_of_iterations,
           averageR / number_of_iterations, averageTotal / delete_num);
}
