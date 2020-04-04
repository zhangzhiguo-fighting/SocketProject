#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main(int argc, char ** argv) {
    int port, server_listen;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s port!\n", argv[0]);
        return 1;
    }
    port = atoi(argv[1]);

    char *filename = argv[2];
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }
    if ((server_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
         return -1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_listen, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");  
        return -1;
      }
 
      if (listen(server_listen, 20) < 0){
          perror("listen");
          return -1;
      }

    while (1) {
        int sockefd, nrd;
        pid_t pid;
        if ((pid = fork()) < 0) {
            perror("fork");
        }
        
        if (pid == 0) {
            if ((sockefd = accept(server_listen, NULL, NULL)) < 0) {
            perror("accept");
            }
            char buffer[BUFFER_SIZE] = {0};
            while((nrd = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
                send(sockefd, buffer, nrd, 0);
            }
            shutdown(sockefd, 0);
            recv(sockefd, buffer, BUFFER_SIZE, 0);
            fclose(fp);
            close(sockefd);
        }
        wait(NULL);
        /*if ((sockefd = accept(server_listen, NULL, NULL)) < 0) {
            perror("accept");
        }
        char buffer[BUFFER_SIZE] = {0};
        while((nrd = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
            send(sockefd, buffer, nrd, 0);
        }
        shutdown(sockefd, 0);
        recv(sockefd, buffer, BUFFER_SIZE, 0);
        fclose(fp);
        close(sockefd);
        //system("pause");*/
    }
    return 0;
}
