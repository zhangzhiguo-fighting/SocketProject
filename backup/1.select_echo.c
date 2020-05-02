






#include"../common/head.h" 
#include"../common/color.h"
#include"../common/common.h"
#include"../common/tcp_server.h"

#define CLIENTSIZE 100
#define BUFFSIZE 512

char ch_char(char c) {
    if(c >= 'a' && c <= 'z') {
        return c - 32;
    }
    return c;
}

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s port!\n", argv[0]);
        exit(1);
    }
    int server_listen, fd, max_fd;
    int client[CLIENTSIZE] = {0};
    memset(client, -1, sizeof(client)); //按字节来置
    if((server_listen = socket_create(atoi(argv[1]))) < 0) {
        perror("socket_create");
        exit(1);
    }

    make_nonblock(server_listen); //

    fd_set rfds, wfds, efds;
    max_fd = server_listen;
    client[server_listen] = server_listen; //先把server_listen 方进去

    while (1) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&efds);

        FD_SET(server_listen, &rfds);

        for (int i = 0; i < CLIENTSIZE; i++) { //轮询， 看那些就绪, 第一次只放入了server_listen
            if (client[i] == server_listen) continue;
            if (client[i] > 0) {
                if (max_fd < client[i]) max_fd = client[i]; //更新max_fd
                FD_SET(client[i], &rfds); //将连接上的客户放在可读集合中
            }
        }
        
        if (select(max_fd + 1, &rfds, NULL, NULL, NULL) < 0) {
            perror("select");
            return 1;
        }

        if (FD_ISSET(server_listen, &rfds)) {
            printf("Connect ready on serverlisten!\n");
            if ((fd == accept(server_listen, NULL, NULL)) < 0) {
                perror("accept");
                return 1;
            }
            if (fd > CLIENTSIZE) {
                printf("Too mant clients!\n");
                close(fd);
            } else {
                make_nonblock(fd);
                if (client[fd] == -1) { //此位置未被使用
                    client[fd] = fd; //使数组下标与文件描述符对应，方便上层操作
                }
            }
            for (int i = 0; i < CLIENTSIZE; i++) {
                if (i == server_listen) continue;
                if (FD_ISSET(i, &rfds)) { //判断可读是否就绪
                    char buff[BUFFSIZE] = {0};
                    int retval = recv(i, buff, BUFFSIZE, 0);
                    if (retval <= 0) {
                        printf("Logout!\n");
                        client[i] = -1;
                        close(i); //退出之前关闭相应的文件描述符
                        continue;
                    }
                    printf("Recv : %s", buff);
                    for (int j = 0; j < strlen(buff); j++) {
                        buff[j] = ch_char(buff[j]);
                    }
                    send(i, buff, strlen(buff), 0);
                }
            }
        }
    }

    return 0;
}

