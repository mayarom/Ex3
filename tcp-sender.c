#include <netdb.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>

#define SA struct sockaddr
#define SERVER_PORT 5060
#define SERVER_IP "127.0.0.1"
#define MAX_LENGTH 1048576 //  max length for the file contents- 1 MB in bytes
int recv_it(int *num, int fd);
int main()
{

    // start reading the file
    FILE *fr = fopen("manulis13615.txt", "r");
    if (fr == false)
    {
        perror("Error to open the file");
        exit(1);
    }

    // file size
    fseek(fr, 0, SEEK_END);
    long lenfr = ftell(fr);
    // rewind function is a possible cancellation point and therefore not marked with __THROW.
    rewind(fr);

    // dividing the file to two parts
    long firstlen = (lenfr / 2);
    long secondlen = lenfr - firstlen;

    // Read the first part of the file
    char *first = malloc(firstlen);
    if (first == false)
    {
        perror("Error in reading the first part of the file");
        fclose(fr);
        return 1;
    }
    fread(first, 1, firstlen, fr);

    // Read the second part of the file
    char *second = malloc(secondlen);
    if (second == false)
    {
        perror("Error in reading the second part of the file");
        fclose(fr);
        return 1;
    }
    fread(second, 1, secondlen, fr);

    // Close the file
    fclose(fr);

    int sockfd;
    struct sockaddr_in servaddr;

    // make a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // check if the socket is created
    if (sockfd == -1)
    {
        printf("Error- the socket is not created\n");
        // exit the program
        exit(1);
    }
    else
    {
        printf("good luck ! the socket created\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    // convert the ip address to binary form
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(SERVER_PORT);

    // check if the connection is established

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        // finish the program if the connection is not established
        printf("connection with the server failed...\n");
        close(sockfd);
        // exit the program
        exit(1);
    }
    else // if the connection is established
    {
        printf("connected to the server\n");
        // the id of the sender
        char choice;
        // the id of the sender
        uint32_t firstid = 5251;
        uint32_t secondid = 9881;
        // xor the two ids
        uint32_t xor = firstid ^ secondid;

        while (1) // while the connection is established
        {
            char *cc = "cubic";

            // check if the cc algorithm is set
            if (setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, cc, strlen(cc)) != 0)
            {
                printf("setsockopt failed \n");
                return -1;
            }

            // send the first part of the file like the way we did in reciever.c
            while (firstlen > 0)
            {
                // find a problem in sending the data
                if (send(sockfd, first, 1, 0) == -1)
                {
                    printf(" there is a problem in sending the data\n");
                    exit(1);
                }

                firstlen = firstlen - 1;
            }
            // if no problem in sending the data
            printf("First part sent \n");

            // send the xor of the ids
            int *renum = (int *)malloc(sizeof(int)); // the number to receive
            recv_it(renum, sockfd);
            printf("Received int = %d\n", *renum); // print the number

            if (*renum != xor) // if the number is not the xor of the ids- error
            {
                printf("Authentication failed \n");
                break;
            }

            char *ren_cc = "reno"; // declare the cc algorithm to reno

            // set the cc algorithm to reno with setsockopt
            int answer = setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, ren_cc, strlen(ren_cc));
            if (answer == -1)
            {
                printf("Error setting the cc algorithm to reno \n");
                break;
            }

            // send the second part of the file
            while (secondlen > 0)
            {
                if (send(sockfd, second, 1, 0) == -1)
                {
                    printf("Error sending the second part \n");
                    return 1;
                }
                secondlen = secondlen - 1;
            }

            printf("Second part sent \n");

            // prompt the user what to do: "Send the file again? (y/n)" and wait for the user's input
            // if the user enters 'y', send the file again
            // if the user enters 'n', break the loop

            printf("do you want to send the file? (y/n): ");
            scanf(" %c", &choice);
            if (choice == 'y')
            {
                // send the file again
                send(sockfd, "send again", 5, 0);
                continue;
            }
            else // if the user enters 'n'
            {
                // finish- close the connection and exit
                send(sockfd, "finish", 4, 0);
                break;
            }

            choice = 0;
        }
    }
    close(sockfd);
    printf("the connection is close\n");

    free(first); // free the first and second part of the file
    free(second);
    printf("free the first and second part of the file");
    return 0;
}

void servermess(char *part, int socket_fd)
{
    double partlen = MAX_LENGTH / 2;
    int total = send(socket_fd, part, partlen, 0);
    if (total == -1)
    {
        printf("send failed  : %d", errno);
        close(socket_fd);
        exit(1);
    }
    else if (total == 0) // in case the connection is closed
    {
        printf("error with the connection \n");
    }
    else if (total < partlen) // if the message is not sent completely
    {
        printf("find a problem- sent only %f bytes from the required %d.\n", partlen, total);
    }
    else // if the message is sent successfully
    {
        printf("the message is sent successfully.\n");
    }
}

int recv_it(int *xorid, int server_sock)
{
    int recv = 0;
    int32_t ret;
    char *data = (char *)&ret;
    int lsize = sizeof(ret);

    do
    {
        // https://stackoverflow.com/questions/9140409/transfer-integer-over-a-socket-in-c
        recv = read(server_sock, data, lsize);
        if (recv <= 0)
        { // error or end of file
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                // try again
            }
            if (errno != EINTR)
            {
                return -1;
            }
        }
        else // if the data is received successfully
        {
            data = data + recv;   // move the pointer to the next data
            lsize = lsize - recv; // decrease the size of the data
        }
    } while (lsize > 0); // if the data is not received completely
    *xorid = ntohl(ret); // convert the data to host byte order
    return 0;
}