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
#include "mystack.h"

#define MAYA 5251
#define YOGEV 9881
#define PORT 5060
#define MAX_LENGTH 1462544 // 1 MB
#define SA struct sockaddr
#define ONE 1

void getChoice(int sockfd, char *choice);
int intReceiver(int *number, int fd);

int main()
{

    // Open the file in read-only mode
    FILE *our_file = fopen("1mb.txt", "r");
    if (our_file == NULL) // if the file is not exist
    {
        perror(" oops, we can't open the file");
        return ONE; // return 1 to indicate an error
    }

    // Move the file position indicator to the end of the file
    int res = fseek(our_file, 0, SEEK_END);
    if (res != 0)
    {
        perror("oops, there is a problem with the file");
        return ONE;
    }
    long fileSize = ftell(our_file);

    // Check that the file position indicator is at the end of the file
    long pos = ftell(our_file);

    if (pos != fileSize)
    {
        perror("oops, there is a problem with the file");
        return ONE;
    }
    long secondPartSize = fileSize / 2;
    long firstPartSize = fileSize / 2;

    rewind(our_file); // return the pointer to the beginning of the file

    // Read the first part of the file
    char *firstPart = malloc(firstPartSize); // make a place in the memory for the first part of the file
    if (!firstPart)
    {
        perror("oops, there is a problem with allocating memory");
        return ONE;
    }
    size_t bytes_read = fread(firstPart, 1, firstPartSize, our_file);

    if (bytes_read == 0) // if there is a problem with reading the file
    {
        perror("oops, there is a problem with reading the file");
        return ONE;
    }

    // Read the second part of the file
    char *secondPart = malloc(secondPartSize);
    if (!secondPart)
    {
        perror("oops, there is a problem with allocating memory");
        fclose(our_file);
        return ONE;
    }
    fread(secondPart, 1, secondPartSize, our_file);

    // Close the file
    fclose(our_file);

    int sockfd;
    struct sockaddr_in servaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) // if socket creation failed then
    {
        printf("Unfortunally socket creation failed...\n");
        return 1;
    }
    else // if socket created successfully then
    {
        printf("good ! we make the socket \n");
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the sender socket to receiver socket
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("oops, the connection with the receiver has been failed ! don't give up, try again\n");
        close(sockfd);
        return ONE;
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
                if (send(sockfd, firstPart, 1, 0) == -1) // send the first part of the file
                {
                    printf("oops' there is a problem with sending the data \n");
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
                printf("error setting the cc algorithm to reno \n");
                break;
            }

            // send the second part of the file
            while (secondPartSize > 0)
            {
                if (send(sockfd, secondPart, 1, 0) == -1)
                {
                    printf("error while sending the secondPart \n");
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

            getChoice(sockfd, &choice);
            if (choice == 'n')
            {
                break;
            }

            choice = 0;
        }
    }
    close(sockfd);
    printf("Connection closed, free first part, free second part \n");

    return 0;
}

int intReceiver(int *number, int fd)
{
    int32_t conv;               // the int32_t variable
    char *data = (char *)&conv; // the char pointer
    int part = sizeof(conv);
    int check; // the check variable
    for (int i = 0; i < part; i += check)
    {
        check = read(fd, data + i, part - i);
        if (check <= 0) // if the check is less or equal to 0
        {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                perror("oops, we find a problem with reading the data");
            }
            if (errno != EINTR) // if the error is not EINTR
            {
                perror("oops, we find a problem with reading the data");
                return -1;
            }
        }
    }
    *number = ntohl(conv);
    return 0;
}

void getChoice(int sockfd, char *choice)
{
    scanf(" %c", choice);
    while (*choice != 'n' && *choice != 'y' && *choice != 'N' && *choice != 'Y')
    {
        printf("Invalid input, try again \n");
        scanf(" %c", choice);
    }
    if (*choice == 'y' || *choice == 'Y')
    {
        // tell the receiver to send the file again
        send(sockfd, "again", 5, 0);
    }
    else
    { // *choice == 'n'
        // Send an exit message to the receiver.
        // The receiver will close the connection and exit
        send(sockfd, "exit", 4, 0);
    }
}
