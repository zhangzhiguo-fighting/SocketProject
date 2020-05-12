#include "../common/color.h"
#include "../common/common.h"
#include "../common/tcp_server.h"
#include "../common/head.h"
#include <sys/ipc.h>
#include <sys/sem.h>

#define MAX 5 //资源数
//#define MAXSEQ 10

union semnum {
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
};

static int total = 0;
static int sem_id = 0;
static int set_semvalue();
static void del_semvalue();
static int semaphore_p();
static int semaphore_v();

void *work(void *arg) {
    int new_fd = *(int *)arg; //int *new_fd = (int *)arg;同一片地址
    semaphore_p();
    char msg[512] = "You can say!\n";
    send(new_fd, msg, strlen(msg), 0);
    while (1) {
        memset(msg, 0, sizeof(msg));
        int rnum = recv(new_fd, msg, sizeof(msg), 0);
        if (rnum <= 0) {
            total --;
            semaphore_v();
            close(new_fd);
            return 0;
        }
        send(new_fd, msg, strlen(msg), 0);
    }
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "error!\n");
        exit(1);
    }

    if ((sem_id = semget((key_t)8731, 1, 0666 | IPC_CREAT)) < 0) {
        perror("semget");
        exit(1);
    }

    if (!set_semvalue()) {
        perror("set_semvalue");
        exit(1);
    }

    int server_listen, fd;
    if ((server_listen = socket_create(atoi(argv[1]))) < 0) {
        perror("socket_creat!");
        exit(1);
    }
    while (1) {
        if ((fd = accept(server_listen, NULL, NULL)) < 0) {
            perror("accept");
            exit(1);
        }
        total++;
        printf("The %dth client!\n", total);
        send(fd, "You are Here!\n", sizeof("You are Here!\n"), 0);

        pthread_t tid;
        pthread_create(&tid, NULL, work, (void *)&fd);
    }

    sleep(100000);

}

static int set_semvalue() {
    union semnum sem_union; //该联合体没有定义在任何系统头文件中，因此得用户自己声明
    sem_union.val = MAX;

    if (semctl(sem_id, 0, SETVAL, sem_union) == -1) { //设置资源数
        return 0;
    }
    return 1;
}

/*
 *int semctl(int semid, int semnum, int cmd, ... );
 semctl() 在 semid 标识的信号量集上，或者该集合的第semnum 个信号量上执行 cmd 指定的控制命令。(信号量集合索引起始于零。)
 * */

static void del_semvalue() {
    union semnum sem_union;

    if (semctl(sem_id, 0, IPC_RMID, sem_union) == -1) {
        perror("semctl_RMID");
    }
}

static int semaphore_p() {
    struct sembuf sem_b;
    sem_b.sem_num = 0; //信号量的编号，如果你的工作不需要使用一组信号量，这个值一般就取为0
    sem_b.sem_op = -1; //信号量一次PV操作时加减的数值，一般只会用到两个值，一个是“－1”，也就是P操作，等待信号量变得可用；另一个是“＋1”，也就是我们的V操作，发出信号量已经变得可用
    sem_b.sem_flg = SEM_UNDO; //通常被设置为SEM_UNDO.她将使操作系统跟踪当前进程对该信号量的修改情况

    if (semop(sem_id, &sem_b, 1) == -1) {
        perror("semop_p");
        return 0;
    }
    return 1;
}

/*   
 *   在 Linux 下，PV 操作通过调用semop函数来实现。该函数定义在头文件 sys/sem.h中，原型如下：
     int  semop（int  semid，struct sembuf  *sops，size_t nsops）；
     函数的参数 semid 为信号量集的标识符；参数 sops 指向进行操作的结构体数组的首地址；参数 nsops 指出将要进行操作的信号的个数。semop 函数调用成功返回 0，失败返回 -1。
     semop 的第二个参数 sops 指向的结构体数组中，每个 sembuf 结构体对应一个特定信号的操作。因此对信号量进行操作必须熟悉该数据结构，该结构定义在 linux/sem.h，如下所示：
     struct  sembuf{
        unsigned short   sem_num;     //信号在信号集中的索引，0代表第一个信号，1代表第二个信号  
        short          sem_op;     //操作类型
        short          sem_flg;   //操作标志
    };    
*/

static int semaphore_v() {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1;
    sem_b.sem_flg = SEM_UNDO;

    if (semop(sem_id, &sem_b, 1) == -1) {
        perror("semop_v");
        return 0;
    }
    return 1;
}




