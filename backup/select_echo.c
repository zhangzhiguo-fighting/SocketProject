#include "../common/color.h"
#include "../common/common.h"
#include "../common/tcp_server.h"
#include "../common/head.h"

#define CLIENTSIZE 50
#define BUFSIZE 4096


struct  Buffer{
    int fd;
    char buff[BUFSIZE];
    int flag; //标记什么时候能发送 1为能发送
    int sendindex;
    int recvindex;
};

struct Buffer *AllocBuffer() { //返回结构体指针，初始化结构体
    struct Buffer *buffer =(struct Buffer *)malloc(sizeof(struct Buffer)); //开辟一个空间的结构体
    if (buffer == NULL) //检测是否开辟成功 
        return  NULL;
    buffer->fd = -1;
    buffer->flag = buffer->recvindex = buffer->sendindex = 0;
    memset(buffer->buff, 0, BUFSIZE);
    return buffer;
}

void FreeBuffer(struct Buffer *buffer) {
    free(buffer);
}




char ch_char(char c) {
    if (c >= 'a' && c <= 'z')
        return c - 32;
    return  c;
}

int RecvToBuffer(int fd, struct Buffer *buffer) {
    //printf("xxx");
    char buff[BUFSIZE];
    int recv_num;
    while (1) {
        recv_num = recv(fd, buff, sizeof(buff), 0);
        if (recv_num <= 0) break;
        for (int i = 0; i < recv_num; i++) {
            if (buffer->recvindex < sizeof(buffer->buff)) //未满，还能接收
                buffer->buff[buffer->recvindex++] = ch_char(buff[i]);
                //printf("%c\n", buff[i]);
                //printf("%c, %c\n", buffer->buff[buffer->recvindex - 1], buffer->buff[buffer->recvindex - 2]);
            if (buffer->recvindex > 1 && buffer->buff[buffer->recvindex - 1] == '\r' && buffer->buff[buffer->recvindex - 2] == '\n') { //怎么知道的？，通过打印最后两个字符
                printf("recvindex - 1 = %d, recvindex - 2 = %d\n", buffer->buff[buffer->recvindex - 1], buffer->buff[buffer->recvindex - 2]);
                buffer->flag = 1;
            }

                //buffer->flag = 1;
        }
    }
    if (recv_num < 0) { //非阻塞IO， 返回-1，不能定就出错，有可能任务还在后面执行没来得及返回
        if (errno == EAGAIN) //再试试，不算错，套接字没问题，里面没有数据
            return 0;
        return -1;
    } else if (recv_num == 0) {
        return 1;
    }
}

int SendFromBuffer(int fd, struct Buffer *buffer) {
    int send_num;
    while (buffer->sendindex < buffer->recvindex){
        send_num = send(fd, buffer->buff + buffer->sendindex, buffer->recvindex - buffer->sendindex, 0);
        if (send_num < 0) {
            if (errno == EAGAIN)
                return 0;
            buffer->fd = -1;
            return -1;
        }
        buffer->sendindex += send_num;
    }
    if (buffer->sendindex == buffer->recvindex)
        buffer->sendindex = buffer->recvindex = 0;
    buffer->flag = 0;
    return 0;
}

int main(int argc, char ** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port!\n", argv[0]);
        exit(1);
    }
    int server_listen, fd, max_fd;
    if ((server_listen = socket_create(atoi(argv[1]))) < 0) {
        perror("socket_create");
        exit(1);
    }
    struct Buffer *buffer[CLIENTSIZE]; //结构体指针数组
    
    for (int i = 0; i < CLIENTSIZE; i++) { //初始化每个结构体
        buffer[i] = AllocBuffer();
    }

    make_nonblock(server_listen);

    fd_set rfds, wfds, efds;
    max_fd = server_listen;

    while (1) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&efds);

        FD_SET(server_listen, &rfds);

        for (int i = 0; i < CLIENTSIZE; i++) {
           if (buffer[i]->fd == server_listen) continue;
            if (buffer[i]->fd > 0) { //有客户端连接上来
                if (max_fd < buffer[i]->fd) max_fd =buffer[i]->fd; //更形max_fd
                FD_SET(buffer[i]->fd, &rfds);
                if (buffer[i]->flag == 1) //
                    FD_SET(buffer[i]->fd, &wfds);
            }
        }

        if (select(max_fd + 1, &rfds, &wfds, NULL, NULL) < 0) {
            perror("select");
            return 1;
        }

        if (FD_ISSET(server_listen, &rfds)) {
            printf("Connect ready on serverlisten!\n");
            if ((fd = accept(server_listen, NULL, NULL)) < 0 ) {
                perror("accept");
                return 1;
            }
            if (fd > CLIENTSIZE) {
                printf("Too many clients!\n");
                close(fd);
            } else {
                printf("Login %d!\n", fd);
                make_nonblock(fd);
                if (buffer[fd]->fd == -1) //次结构体以前未被使用
                buffer[fd]->fd = fd;
            }
        }
 
        for (int i = 0; i < max_fd + 1; i++) {
            int retval;
            if (i == server_listen || buffer[i]->fd < 0) continue;
            //printf("Before Recv!");
            if (FD_ISSET(i, &rfds)) {
                retval = RecvToBuffer(i, buffer[i]);
            }
            //printf("Before Send!");
            if (retval == 0 && FD_ISSET(i, &wfds)) { // 已经接受完，并且具有了可写的权力，把信息发送回去
                retval = SendFromBuffer(i, buffer[i]);
            } 
            if (retval) {
                printf("Logout !\n");
                buffer[i]->fd = -1;
                close(i);
            }
        }
    }

    return 0;
} 
