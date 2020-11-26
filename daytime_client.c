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

int main(int argc, char **argv) {

    int sockfd;
    char buffer[MSGLENGTH];
    int n = 0;

    struct sockaddr_in server_addr;
    struct in_addr     server_ip;
    int                server_port = 13;

    int help;
    int opt;

    // Loopback IP: 127.0.0.1 (default value)
    server_ip.s_addr = htonl(INADDR_LOOPBACK);

    /*
     * Parsing input arguments, possible, all optional arguments:
     * -h: Outputs a help message.
     * -i: IP address of the DAYTIME server.
     * -p: Port of the DAYTIME server.
     * Default: localhost:13
     */
    while ((opt = getopt(argc, argv, "i:p:h")) != -1) {
        switch(opt) {
            case 'i':
                inet_aton(optarg, &server_ip);
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

    /*
     * Assemble together server IP and server port we want to connect to.
     */
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = server_ip.s_addr;

    /*
     * Connecting to the server via socket sockfd.
     */
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        return -1;
    } else 
        printf("Connection to %s:%d successful!\n",inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));

    /*
     * Receiving the DAYTIME message from the sever.
     */
    while ((n = read(sockfd, buffer, MSGLENGTH-1)) > 0) {

        buffer[n] = 0;
        printf("Daytime received: %s\n",buffer);
    }

    return 0;
}