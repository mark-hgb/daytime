#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>

/*
 * A DAYTIME message from this server is 26 bytes long, including a closing \r\n.
 * 
 * Example: Thu Nov 26 11:29:54 2020\r\n
 */
#define MSGLENGTH 27    

int main(int argc, char **argv)
{
    int     sockfd;
    int     connfd;

    int     opt;
    int     help;

    struct  sockaddr_in server_addr;
    struct  sockaddr_in client_addr;
    int                 client_addr_len = sizeof(struct  sockaddr_in);
    unsigned int        server_port = 13;
    struct  in_addr     server_ip;
    
    time_t  curr_time;
    char    buffer[MSGLENGTH];

    /*
     * Parsing input arguments, possible, all optional arguments:
     * -h: Outputs a help message.
     * -i: Local IP address the server is listening on.
     * -p: Local port the server is listening on.
     * Default: localhost:13
     */
    while ((opt = getopt(argc, argv, "i:p:h")) != -1) {
        switch(opt) {
            case 'i':
                if (inet_aton(optarg, &server_ip) == 0)
                    server_ip.s_addr = htonl(INADDR_ANY);

                break;
            case 'p':
                help = atoi(optarg);

                if (help > 0 && help < 65535)
                    server_port = help;
                else
                    printf("Incorrect port number, using %d ...\n", server_port);
                break;
            case 'h':
                fprintf(stderr, "Usage: %s [-i IP] [-p PORT]\n", argv[0]);
                return -1;
            default:
                fprintf(stderr, "Usage: %s [-i IP] [-p PORT]\n", argv[0]);
                return -1;
        }
    }

    /* 
     * Create a socket for connection oriented and reliable transport (SOCK_STREAM),
     * which means a TCP socket. In sockfd we get a file descriptor used to access
     * the socket.
     */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        return -1;
    } else
        printf("Socket successfully created!\n");

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1) {
        perror("Operation setsockopt(SO_REUSEADDR) failed");
    }

    /*
     * Assemble together server IP and server port.
     */
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = server_ip.s_addr;
    server_addr.sin_port = htons(server_port);

    /*
     * Bind the socket sockfd to the specified server IP and port.
     */
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding socket failed");
        return -1;
    } else 
        printf("Binding socket successfully!\n");

    /*
     * Open the socket sockfd in listen mode (server socket).
     */
    if (listen(sockfd, 256) == -1) {
        perror("Listening on socket failed");
        return -1;
    } else 
        printf("Listening on socket ...!\n\n");

    /*
     * Typical single threaded server implementation by handling incoming connections in an infinite loop.
     */
    for (;;) {

        /*
         * Accepting incoming connections on socket sockfd. The structure client_addr contains the 
         * IP address and port of the connecting client. The file descriptor connfd represents the
         * connection with the client.
         */
        if ((connfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
            perror("New incoming connection failed");
            return -1;
        } else 
            printf("Incoming connection from %s:%d successful!\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
        
        /*
         * Getting a so called UNIX timestamp (seconds elapsed since January 1st, 1970).
         */
        curr_time = time(NULL);
        printf("Timestamp to send: %s",ctime(&curr_time));
        snprintf(buffer, sizeof(buffer), "%.24s\r\n", ctime(&curr_time));

        /*
         * Sending the timestamp on socket connfd.
         */
        if (write(connfd, buffer, strlen(buffer)) == -1) {
            perror("Writing data on socket failed");
            return -1;
        } else 
            printf("Writing data on socket successful!\n");

        /*
         * Closing the connection via socket connfd
         */
        if (close(connfd) == -1) {
            perror("Closing connection failed");
            return -1;
        } else 
            printf("Closing connection to %s:%d successfull!\n\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
    }
}