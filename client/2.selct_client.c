/*************************************************************************
	> File Name: client.c
	> Author: suyelu
	> Mail: suyelu@haizeix.com
	> Created Time: 六  3/28 14:51:21 2020
 ************************************************************************/


#include "head.h"
#include "tcp_client.h"

int main(int argc, char **argv) {
    char msg[512] = {0};
    int sockfd;
    if (argc != 4) {
        fprintf(stderr, "Usage: %s ip port\n", argv[0]);
        return 1;
    }

    if ((sockfd = socket_connect_timeout(argv[1], atoi(argv[2]), atoi(argv[3]))) < 0) {
        perror("socket_connect");
        return 2;
    }
    printf("socket_connect!\n");
    //sleep(60);
    close(sockfd);
    return 0;
} 
