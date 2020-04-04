#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char ** argv) {
    int port, sockefd, nrd;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s port!\n", argv[0]);
        return 1;
    }
    port = atoi(argv[2]);

    char filename[100] = {0};
    printf("Input filename to save:");
    scanf("%s", filename);
    //getchar();
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
    if ((sockefd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
         return -1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockefd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        return -1;
    }

    char buffer[BUFFER_SIZE] = {0};
    while((nrd = recv(sockefd, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, nrd, 1, fp);
    }
    puts("File transfer success!");
    fclose(fp);
    close(sockefd);
    return 0;
}
