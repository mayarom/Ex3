#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <time.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include "unistd.h"

#define MAYA 5251
#define YOGEV 9881
#define PORT 5030
#define MAX_LENGTH 1462544 // 1 MB
#define SA struct sockaddr

int intReceiver(int *number, int fd);

int main()
{

    // Open the file in read-only mode
    FILE *file = fopen("1mb.txt", "r");
    if (!file)
    {
        perror("Error opening file");
        return 1;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // Calculate the size of the first and second parts
    long secondPartSize = fileSize / 2;
    long firstPartSize = fileSize / 2;

    // Read the first part of the file
    char *firstPart = malloc(firstPartSize);
    if (!firstPart)
    {
        perror("Error allocating memory");
        fclose(file);
        return 1;
    }
    fread(firstPart, 1, firstPartSize, file);

    // Read the second part of the file
    char *secondPart = malloc(secondPartSize);
    if (!secondPart)
    {
        perror("Error allocating memory");
        fclose(file);
        return 1;
    }
    fread(secondPart, 1, secondPartSize, file);

    // Close the file
    fclose(file);

    int sockfd;
    struct sockaddr_in servaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)// if socket creation failed then
    {
        printf("Unfortunally socket creation failed...\n");
        return 1;
    }
    else // if socket created successfully then
    {
        printf("Socket successfully created..\n");

    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the sender socket to receiver socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Unfortunally the connection with the receiver has been failed failed...\n");
        close(sockfd);
        return 1;
    }
    else
    {
        printf("Great! connected to the receiver..\n");
        char choice;
        uint32_t XOR = MAYA ^ YOGEV;

        while (1)
        {

            // set the cc algorithm to cubic with setsockopt
            char *cc = "cubic";
            if (setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, cc, strlen(cc)) != 0)
            {
                printf("setsockopt failed \n");
                return -1;
            }

            // send the first part of the file like the way we did in reciever.c
            while (firstPartSize > 0)
            {
                if (send(sockfd, firstPart, 1, 0) == -1)
                {
                    printf("Error while sending data \n");
                    return -1;
                }
                firstPartSize = firstPartSize - 1;
            }

            printf("First part has been sent \n");
            // TODO recieve the authentication from the receiver
            // we get the XOR message from the receiver

            // init a int pointer
            int *number = (int *)malloc(sizeof(int));
            intReceiver(number, sockfd);
            printf("Received int = %d\n", *number);
            if (*number != XOR)
            {
                printf("Authentication has been failed \n");
                break;
            }
            else
            {
                printf("Authentication has succeed! \n");
            }

            char *ccAlgorithm = "reno"; // the CC algorithm to use (in this case, "reno")

            // set the cc algorithm to reno with setsockopt

            int return_value = setsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, ccAlgorithm, strlen(ccAlgorithm));
            if (return_value == -1)
            {
                printf("Error setting the cc algorithm to reno \n");
                break;
            }

            // send the second part of the file
            while (secondPartSize > 0)
            {
                if (send(sockfd, secondPart, 1, 0) == -1)
                {
                    printf("Error while sending the secondPart \n");
                    break;
                }
                secondPartSize = secondPartSize - 1;
            }

            printf("Second part sent \n");

            printf("*** you are lucky! you got all the file ! *** \n");

            // prompt the user what to do: "Send the file again? (y/n)" and wait for the user's input
            // if the user enters 'y', send the file again
            // if the user enters 'n', break the loop
            printf("Send the file again? (y/n): ");

            scanf(" %c", &choice);
            if (choice == 'y')
            {
                // tell the receiver to send the file again
                send(sockfd, "again", 5, 0);
                continue;
            }
            if (choice != 'n' && choice != 'y')
            {
                printf("Invalid input \n");
                break;
            }
            {
                // Send an exit message to the receiver.
                // The receiver will close the connection and exit
                send(sockfd, "exit", 4, 0);
                break;
            }

            choice = 0;
        }
    }
    close(sockfd);
    printf("Connection closed, free first part (%p), free second part (%p)\n", firstPart, secondPart);
    free(firstPart);
    free(secondPart);

    return 0;
}

int intReceiver(int *number, int fd)
{
    int32_t ret;
    char *data = (char *)&ret;
    int left = sizeof(ret);
    int rc;
    for (int i = 0; i < left; i += rc)
    {
        rc = read(fd, data + i, left - i);
        if (rc <= 0)
        {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                // use select() or epoll() to wait for the socket to be readable again
            }
            else if (errno != EINTR)
            {
                return -1;
            }
        }
    }
    *number = ntohl(ret);
    return 0;
}