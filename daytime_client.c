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

int main(int argc, char **argv) {

    int sockfd;
    char buffer[27];
    int n = 0;

    struct sockaddr_in server_addr;
    struct in_addr     server_ip;

    int server_port = 13;
    int help;

    int opt;

    server_ip.s_addr = htonl(INADDR_LOOPBACK);

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

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 6)) == -1) {
        perror("Socket creation failed");
        return -1;
    } else
        printf("Socket successfully created!\n");

    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = server_ip.s_addr;

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            perror("Connection failed");
            return -1;
        } else 
            printf("Connection to %s:%d successful!\n",inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));

    while ((n = read(sockfd, buffer, 27)) > 0) {

        buffer[n] = 0;
        printf("Daytime received: %s",buffer);
    }

    return 0;
}