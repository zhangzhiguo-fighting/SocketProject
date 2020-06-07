#include<stdio.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>


int main(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr, "Usage: %s ip oprt\n",argv[0]);
        exit(1);
    }
    int sockfd, port, n = 0;
    struct sockaddr_in server;
    char buff[512];
   if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket");
        exit(1);
    }
    port = atoi(argv[2]);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(argv[1]);
    
    while(1){
        fgets(buff, sizeof(buff), stdin);
        if((n = sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr*)&server, sizeof(server) )) < 0){
                perror("sendto");
        }
        if((n = recvfrom(sockfd, buff, sizeof(buff), 0, NULL ,0 )) < 0){
            perror("recvfrom");
        }
            printf("buff: %s", buff);
        

    }

    return 0;
}
