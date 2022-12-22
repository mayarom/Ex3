#include <sys/time.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <limits.h>
#include <sys/types.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "mystack.h"
#include <stdio.h>

#define PORT 5060          // port for the sender
#define MAYA 5251          // first id
#define YOGEV 9881         // second id
#define CON_IN_STK 100     // how many pending connections Stack will hold
#define fileSize 1462544   // 1 MB
#define BUFFERSIZE 1462544 // 1 MB
#define receiverPORT 5060  // port for the receiver
#define ONE 1

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

char message[BUFFERSIZE];                                   // the message we will send
void ConnectionSteps(int senderSocket, int receiverSocket); // function who deals with the connection
int SendFileData(int number, int fd);                       //
void PrintTimeData(int number_of_iterations);

int main(int argc, char **argv)
{

    int receiverSocket;               // the socket of the receiver
    int senderSocket;                 // the socket of the sender
    int addrSize;                     //
    SA_IN receiver_addr, sender_addr; // the address of the receiver and the sender

    if ((receiverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) // create the socket
    {
        perror("oops, socket failed"); // if the socket failed
        exit(ONE);
    }
    else // if the socket created successfully
    {
        printf("OMG!the socket created successfully\n");
    }

    // initialize the adress struct
    receiver_addr.sin_family = AF_INET;           // host byte order
    receiver_addr.sin_addr.s_addr = INADDR_ANY;   // automatically fill with my IP
    receiver_addr.sin_port = htons(receiverPORT); // short, network byte order

    // bind the socket to the port
    if (bind(receiverSocket, (SA *)&receiver_addr, sizeof(receiver_addr)) == -1)
    {
        perror("oops, bind failed, you should try again"); // if the bind failed
        exit(ONE);
    }
    // listen to the socket
    if (listen(receiverSocket, CON_IN_STK) == -1) // if the listen failed
    {
        perror("oops, listen failed, you should try again");
        exit(ONE);
    }

    while (1)
    {
        printf("the socket is waiting for connections... \n");
        // wait for and eventually accept an incomming connection

        addrSize = sizeof(SA_IN);
        if ((senderSocket = accept(receiverSocket, (SA *)&sender_addr, (socklen_t *)&addrSize)) == -1)
        {

            printf("socket has been closed, exiting the program\n");

            exit(ONE);
        }
        printf("connected succesfully! on socket %d \n", senderSocket);

        // the the thing we want to do with the connection
        ConnectionSteps(senderSocket, receiverSocket);
    }
    close(receiverSocket);
    printf("closing the receiver socket");
}

void ConnectionSteps(int senderSocket, int receiverSocket)
{
    // recieve the first part of the file

    uint32_t XOR = MAYA ^ YOGEV;
    int interationCounter = 0;
    int numOfBytes = 0;

    while (ONE)
    {
        struct timeval start_t_cubic, end_t_cubic, timeResultCubic;
        struct timeval startReno, endReno, timeResultReno;

        interationCounter++;
        bzero(message, BUFFERSIZE);

        // set the algorithm to cubic
        char *cc = "cubic";
        if (setsockopt(senderSocket, IPPROTO_TCP, TCP_CONGESTION, cc, strlen(cc)) > 0 || setsockopt(senderSocket, IPPROTO_TCP, TCP_CONGESTION, cc, strlen(cc)) < 0)
        {
            printf("setsockopt has been failed \n");
            return;
        }
        printf("CC set to %s\n", cc);

        gettimeofday(&start_t_cubic, NULL); // starts the time

        for (; numOfBytes < BUFFERSIZE / 2; numOfBytes++)
        {
            if (recv(senderSocket, message, 1, 0) == -1)
            {
                printf("receive has been failed \n");
                break;
            }
        }
        usleep(1000);                     // sleep for 1 second
        gettimeofday(&end_t_cubic, NULL); // finish count for first part of the file

        bzero(message, BUFFERSIZE);                               // clear the buffer
        timersub(&end_t_cubic, &start_t_cubic, &timeResultCubic); // the total time cubic

        long int *timeElapseCubic = (long int *)malloc(sizeof(long int));
        *timeElapseCubic = timeResultCubic.tv_sec * 1000000 + timeResultCubic.tv_usec;
        int *interationNumberPoint = (int *)malloc(sizeof(int));
        *interationNumberPoint = interationCounter;
        int *cubicParam = (int *)malloc(sizeof(int));
        *cubicParam = 0;
        push(timeElapseCubic, interationNumberPoint, cubicParam);

        printf("algo: cubic, time: %ld.%06ld, iter number: %d\n",
               (long int)timeResultCubic.tv_sec,
               (long int)timeResultCubic.tv_usec,
               interationCounter);

        // Send back the authentication to the sender.
        // send thhe XOR to the sender
        printf("sending the XOR to the sender %d \n", XOR);
        SendFileData(XOR, senderSocket);

        bzero(message, BUFFERSIZE);

        // change the algorithm to reno
        char *ccAlgorithm = "reno"; // the CC algorithm to use (in this case, "reno")
        if (setsockopt(receiverSocket, IPPROTO_TCP, TCP_CONGESTION, ccAlgorithm, strlen(ccAlgorithm)))
        {
            perror("setsockopt failed");
            exit(1);
        }

        printf("CC set to %s\n", ccAlgorithm); // print the CC algorithm

        // recive the second part of the file
        gettimeofday(&startReno, NULL); // start the time

        // recive a file of half mega bytes
        while (numOfBytes < BUFFERSIZE)
        {
            if (recv(senderSocket, message, 1, 0) == -1) // if the recive failed
            {
                printf("oops, recive failed... you should try again \n");
                break;
            }
            numOfBytes++; // increase the number of bytes
        }
        usleep(1000);
        gettimeofday(&endReno, NULL);                    // finish count for first part of the file
        timersub(&endReno, &startReno, &timeResultReno); // the total time reno

        printf("algo: reno, time: %ld.%06ld, iter number: %d\n", (long int)timeResultReno.tv_sec, (long int)timeResultReno.tv_usec, interationCounter);

        // store the time elapsed in a variable
        long int timeElapseReno = timeResultReno.tv_sec * 1000000 + timeResultReno.tv_usec;
        long int *timeElapseRenoP = (long int *)malloc(sizeof(long int));
        *timeElapseRenoP = timeElapseReno;
        int *renoParam = (int *)malloc(sizeof(int));
        *renoParam = 1;

        push(timeElapseRenoP, interationNumberPoint, renoParam); // push the data to the Stack

        // if you get the exit message from the sender, close the socket and exit
        recv(senderSocket, message, 1024, 0);
        // if the sender send the message "again" the receiver will recive the file again
        if (strcmp(message, "again") == 0)
        {
            continue; // recive the file again
        }
        else
        {
            // print out the report
            printf("\n\n");
            printf("the Execution time report \n");
            PrintTimeData(interationCounter);
            close(senderSocket);
            close(receiverSocket);
            printf("closing the sender socket \n");
            printf("closing the receiver socket \n");
            return;
        }
    }
}

int SendFileData(int number, int fd)
{

    int32_t conv = htonl(number); // convert the number to network byte order
    int total_sent = 0;           // counting the total bytes sent
    char *data = (char *)&conv;   // the data to send
    int sent;

    // send the number to the sender
    while (total_sent < sizeof(conv))
    {
        sent = write(fd, data + total_sent, sizeof(conv) - total_sent);
        if (sent < 0)
        {
            RecErr(sent);
        }
        else
        {
            total_sent = total_sent + sent;
        }
    }

    return 0;
}
void RecErr(int sent)
{
    if (errno == EAGAIN || errno == EWOULDBLOCK)
    {
        printf(" sorry, there is a problem with sending");
        return -1;
    }
    if (errno != EINTR)
    {
        printf(" sorry, there is a problem with sending");
        return -1;
    }

    else if (sent == 0)
    {
        printf("we didn't send anything");
        return -1;
    }
    return 0;
}

void PrintTimeData(int number_of_iterations)
{
    long int averageC = 0;
    long int averageR = 0;
    long int averageTotal = 0;
    long int numberOfDeq = 0;

    // remove from the stack and print the data
    while (first != NULL)
    {
        if (*first->cubic_is_0_reno_is_1 == 0)
        {
            averageC += *first->timeInMicroSeconds;
        }
        else if (*first->cubic_is_0_reno_is_1 == 1)
        {
            averageR += *first->timeInMicroSeconds;
        }
        averageTotal += *first->timeInMicroSeconds;
        pop();
        numberOfDeq++;
    }
    // print out the report
    printf("*** the result report:  **\n");
    printf("The average time for cubic is %ld microseconds\n", averageC / number_of_iterations); // print the average time for cubic
    printf("The average time for reno is %ld microseconds\n", averageR / number_of_iterations);  // print the average time for reno
    printf("The average time for total is %ld microseconds\n", averageTotal / numberOfDeq);      // print the average time for total
    printf("The number of iterations is %d \n", number_of_iterations);                           // print the number of iterations
    if (averageC > averageR)                                                                     // print the winner
    {
        printf("the winner is: cubic \n");
    }
    else if (averageC < averageR)
    {
        printf("Congratulations, the winner is: reno \n");
    }
    else
    {
        printf("Congratulations, the winner is: no one, they are equal \n"); // print the winner
    }
}