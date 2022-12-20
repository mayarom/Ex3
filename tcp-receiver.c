
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "mystack.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <time.h>
#include <sys/socket.h>

#include <netinet/tcp.h>

#define MAX_CON 5 // 100 is the max number of pending connections
#define fileSize 1746760
#define BUFFERSIZE 1746760
#define SERVERPORT 5060

typedef struct sockaddr_in SOCK_IN;
typedef struct sockaddr SA;

char message[BUFFERSIZE];
void handle_connection(int client_sock, int server_sock);
int sendIT(int num, int fd);
void current_situation(int times);

int main(int argc, char **argv)
{
    int server_sock, client_sock, addr_size;
    SOCK_IN serv_addr, clnt_addr;

    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Failed to craete socket\n");
        exit(-1);
    }

    // initialize the adress struct
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SERVERPORT);

    if (bind(server_sock, (SA *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("Failed to bind the socket\n");
        exit(-1);
    }
    if (listen(server_sock, MAX_CON) == -1)
    {
        perror("sorry, failed to listen\n");
        close(server_sock);
        exit(-1);
    }

    while (true)
    {
        printf("Waiting for connection.. \n");
        // wait for and eventually accept an incomming connection

        addr_size = sizeof(SOCK_IN);
        client_sock = accept(server_sock, (SA *)&clnt_addr, (socklen_t *)&addr_size);
        if (client_sock == -1)
        {
            perror("sorry, failed to accept the connection\n");
            close(server_sock);
            exit(-1);
        }

        printf("connected succesfully! on socket %d \n", client_sock);

        // the the thing we want to do with the connection
        // handle_connection(client_sock, server_sock);

        // recieve the first part of the file will use them to check the timing

        uint32_t firstid = 5251;
        uint32_t secondid = 9881;
        uint32_t xor = firstid ^ secondid;
        int inter_count = 0;

        int bytecounter = 0;
        while (1) //  check the timing
        {
            struct timeval startCubic, endCubic, tvalCubic; // will use them to check the timing
            struct timeval startReno, endReno, tvakReno;    // will use them to check the timing

            inter_count = inter_count + 1;
            bzero(message, BUFFERSIZE); // clear the buffer

            // set the algorithm to cubic
            char *cc = "cubic";
            if (setsockopt(client_sock, IPPROTO_TCP, TCP_CONGESTION, cc, strlen(cc)) != 0)
            {
                printf("setsockopt unfortunately failed! \n");
                close(server_sock);
                return 1;
            }
            printf("CC algorithm changed to %s\n", cc);

            if (gettimeofday(&startCubic, NULL) < 0) // start counting the time and check it's not under than zero
            {
                printf("time can't be less than 0 \n");
                break;
            }

            while (bytecounter < BUFFERSIZE / 2) // send the first part of the file
            {
                if (recv(client_sock, message, 1, 0) == -1)
                {
                    printf("failed to recieve \n");
                    break;
                }
                bytecounter = bytecounter + 1;
            }
            usleep(1000);
            gettimeofday(&endCubic, NULL); // finish count for first part of the file

            bzero(message, BUFFERSIZE);
            timersub(&endCubic, &startCubic, &tvalCubic); // the total time cubic

            long int *time_elapsed_cubic = (long int *)malloc(sizeof(long int));
            *time_elapsed_cubic = tvalCubic.tv_sec * 1000000 + tvalCubic.tv_usec;
            int *iteration_number_p = (int *)malloc(sizeof(int));
            *iteration_number_p = inter_count;
            int *cubic_param = (int *)malloc(sizeof(int));
            *cubic_param = 0;
            enqueue(time_elapsed_cubic, iteration_number_p, cubic_param);

            printf("The algorithm is cubic, time: %ld.%06ld, iter num: %d\n",
                   (long int)tvalCubic.tv_sec,
                   (long int)tvalCubic.tv_usec,
                   inter_count);

            printf("sending the xor : %d \n", xor);

            sendIT(xor, client_sock); // send the xor

            bzero(message, BUFFERSIZE);

            // change the algorithm to reno
            char *cc_algo = "reno"; // the CC algorithm to use (in this case, "reno")

            if (setsockopt(server_sock, IPPROTO_TCP, TCP_CONGESTION, cc_algo, strlen(cc_algo)) == -1)
            {
                perror("there is a problem to set the socket! \n");
                close(server_sock);
                return 1;
            }

            printf("we change the CC algorithm  to %s\n", cc_algo);

            // recive the second part of the file
            gettimeofday(&startReno, NULL); // start the time

            // recive a file of half mega bytes
            while (bytecounter < BUFFERSIZE) // send the second part of the file
            {
                if (recv(client_sock, message, 1, 0) == -1)
                {
                    printf("recive failed \n");
                    break;
                }
                bytecounter = bytecounter + 1;
            }
            usleep(1000); // wait for 1 ms

            gettimeofday(&endReno, NULL);
            timersub(&endReno, &startReno, &tvakReno); // the total time reno

            printf("the reno algorithm took : %ld.%06ld, iter num: %d\n", (long int)tvakReno.tv_sec,
                   (long int)tvakReno.tv_usec, inter_count);

            // store the time elapsed in a variable

            long int elapsedTimeReno = tvakReno.tv_sec * 1000000 + tvakReno.tv_usec;
            long int *elapsedTimeReno_P = (long int *)malloc(sizeof(long int));
            *elapsedTimeReno_P = elapsedTimeReno;
            int *reno_param = (int *)malloc(sizeof(int));
            *reno_param = 1;
            enqueue(elapsedTimeReno_P, iteration_number_p, reno_param);

            // if you get the exit message from the client, close the socket and exit
            recv(client_sock, message, 10, 0); // recive the message from the client
            printf("if we choose yes, suppose to be %s \n", message);
            // if the client send the message "again" the server will recive the file again
            if (strcmp(message, "a") == 0)
            {
                continue;
            }
            else
            {
                // print out the report
                printf("\n\n");
                printf("The current result is: \n");
                current_situation(inter_count);
                close(client_sock); // close the socket
                printf("client socket has been closed\n");
                return 1;
            }
        }
    }
    close(server_sock);
    printf("server socket has been closed");
}
// https://stackoverflow.com/questions/9140409/transfer-integer-over-a-socket-in-c
int sendIT(int xor_id, int client_sock)
{
    int32_t conv = htonl(xor_id);
    char *data = (char *)&conv;
    int lpart = sizeof(conv);
    int rc;
    do
    {
        rc = write(client_sock, data, lpart);
        if (rc < 0)
        {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                // use select() or epoll() to wait for the socket to be writable again
            }
            else if (errno != EINTR)
            {
                return -1;
            }
        }
        else
        {
            data += rc;
            lpart = lpart - rc;
        }
    } while (lpart > 0);
    return 0;
}

void current_situation(int times)
{
    long int avgC = 0;         // average time for cubic
    long int avgR = 0;         // average time for reno
    long int avgTotal = 0;     // average time for total
    long int dequ_counter = 0; // counter for the dequeue

    // dequeue the queue and print out the report
    while (head != NULL) // while the queue is not empty
    {

        if (*head->cubic_is_0_reno_is_1 == 1) // if the algorithm is reno
        {
            avgR += *head->time_in_micro_seconds;
        }
        if (*head->cubic_is_0_reno_is_1 == 0) // if the algorithm is cubic
        {
            avgC = avgC + *head->time_in_micro_seconds; // add the time to the cubic average
        }
        avgTotal += *head->time_in_micro_seconds; // add the time to the total average
        dequeue();                                // dequeue the queue
        dequ_counter = dequ_counter + 1;          // increase the counter
    }

    printf("the results: \n\n");

    printf("the average time for cubic algorithm is %ld \n", avgC / times);
    printf("the average time for reno algorithm is %ld \n", avgR / times);
    printf("the average time for total algorithm is %ld \n", avgTotal / dequ_counter);
}
