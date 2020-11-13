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

int main(int argc, char **argv)
{
    unsigned int     server_port = 13;
    struct in_addr   server_ip;
    //int     proto = 6;
    int     sockfd;
    int     connfd;

    int     opt;
    int     help;

    struct  sockaddr_in server_addr;
    struct  sockaddr_in client_addr;
    int     client_addr_len = sizeof(struct  sockaddr_in);
    
    time_t  curr_time;
    char    buffer[27];

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


    if ((sockfd = socket(AF_INET, SOCK_STREAM, 6)) == -1) {
        perror("Socket creation failed");
        return -1;
    } else
        printf("Socket successfully created!\n");

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1) {
        perror("Operation setsockopt(SO_REUSEADDR) failed");
    }

    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = server_ip.s_addr;
    server_addr.sin_port = htons(server_port);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding socket failed");
        return -1;
    } else 
        printf("Binding socket successfully!\n");

    if (listen(sockfd, 256) == -1) {
        perror("Listening on socket failed");
        return -1;
    } else 
        printf("Listening on socket successfully!\n");

    for (;;) {

        if ((connfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
            perror("New incoming connection failed");
            return -1;
        } else 
            printf("Incoming connection from %s:%d successful!\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
        
        curr_time = time(NULL);

        printf("Timestamp to send: %s\n",ctime(&curr_time));

        snprintf(buffer, sizeof(buffer), "%.24s\r\n", ctime(&curr_time));

        if (write(connfd, buffer, strlen(buffer)) == -1) {
            perror("Writing data on socket failed");
            return -1;
        } else 
            printf("Writing data on socket successful!\n");

        if (close(connfd) == -1) {
            perror("Closing connection failed");
            return -1;
        } else 
            printf("Closing connection to %s:%d successfull!\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
    }
}