#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> //getopt
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <termios.h>
#include <sys/ioctl.h>


int terminal_c;
int terminal_r;

int maxlen = 0;

#define CNT 256

int file_name(DIR *fp, char *path, char name[][CNT], int flag);
void str_sort(char name[][CNT], int cnt);
void mode_to_char(mode_t mode, char *buf);
char *time_change(char *time);
void show(char name[][CNT], int cnt);
void size_terminal(int *col, int *row);
void output_format(char name[][CNT], int cnt);


int file_name(DIR*fp,char*path,char name[][CNT], int flag) {
    int cnt=0;
    struct dirent *p;
    if ((fp = opendir(path)) == NULL) {
        perror("opendir");
        return 0;
    }
    printf("%s:\n", path);
    while((p = readdir(fp))!=NULL) {
        if (strlen(p->d_name) > maxlen)
            maxlen = strlen(p->d_name);
        if (flag) {
        if(strncmp(p->d_name,".",1)==0||strncmp(p->d_name,"..",2)==0)
        continue;
        }
        strcpy(name[cnt],p->d_name);
        cnt++;
    }
    closedir(fp);
    return cnt;
}

void str_sort(char name[][CNT], int cnt) {
    char temp[CNT];
    for (int i = 0; i < cnt; i++) {
        for (int j = 0; j < cnt - i - 1; j++) {
            if (strcmp(name[j], name[j + 1]) > 0) {
                strcpy(temp, name[j]);
                strcpy(name[j], name[j + 1]);
                strcpy(name[j + 1], temp);
            }
        }
    }
}

//get information of quanxian
void mode_to_char (mode_t mode, char *buf)
 {
     memset(buf, '-', 10);
     if(S_ISDIR(mode))
         buf[0] = 'd';
     if(mode & S_IRUSR)
         buf[1] = 'r';
     if(mode & S_IWUSR)
         buf[2] = 'w';
     if(mode & S_IXUSR)
         buf[3] = 'x';
     if(mode & S_IRGRP)
         buf[4] = 'r';
     if(mode & S_IWGRP)
         buf[5] = 'w';
     if(mode & S_IXGRP)
         buf[6] = 'x';
     if(mode & S_IROTH)
         buf[7] = 'r';
     if(mode & S_IWOTH)
         buf[8] = 'w';
     if(mode & S_IXOTH)
         buf[9] = 'x';
     buf[10] = '\0';
}


//short number of number
char *time_change(char *time) {
     int index = strlen(time) - 1;
     for(; time[index] != ':'; index --);
     time[index] = '\0';
     return time + 4;
}


 void show(char name[][CNT], int cnt)
 {
     int index;
     char mode[11];
     char *str;
     struct stat buf;
     for(index = 0; index < cnt; index ++)
     {
         memset(&buf, 0, sizeof(buf));
         if(stat(name[index], &buf) == -1)
         {
             printf("stat error!!\n");
             return ;
         }
         mode_to_char(buf.st_mode, mode);
         str = ctime(&buf.st_mtime);
         str = time_change(str);
         printf("%10s %2d %5s %5s %5d %13s %s\n", mode, buf.st_nlink, getpwuid(buf.st_uid)->pw_name, getgrgid(buf.st_gid)->gr_name, buf.st_size, str, name[index]);
     }
}


//size of terminal
void size_terminal(int *col, int *row) {
    struct winsize size;
    ioctl(STDIN_FILENO,TIOCGWINSZ,&size);
    *col = size.ws_col;
    *row = size.ws_row;
    return ;
}

void output_format(char name[][CNT], int cnt) {
    /*int C = terminal_c / maxlen;
    int R = (cnt + 1 / C);
    int distan = 0;
    for (int i = 0; i < cnt; i++) {
        if (distan < terminal_c) {
            printf("%-10s ", name[i]);
        } else {
            printf("\n");
        }
    }
    printf("\n");*/
    for (int i = 0; i < cnt; i++) {
        printf("%s\n", name[i]);
    }
}

int main(int argc, char *argv[]) {
    size_terminal(&terminal_c, &terminal_r);
    DIR *fp;
    char name[CNT][CNT];
    int cnt;
    const char *optstring="::a::l::";
    int opt = 0;
    int flag = 1;
     if (argc == 1) {
         cnt = file_name(fp, ".", name, flag);
         str_sort(name, cnt);
         output_format(name, cnt);
     } else {
        while ((opt = getopt(argc, argv, optstring)) != -1) {
            switch(opt) {
            case 'a':
                flag = 0;
                if (argv[optind] == NULL) {    
                    cnt = file_name(fp, ".", name, flag);
                } else {
                    cnt = file_name(fp, argv[optind], name, flag);
                }
                str_sort(name, cnt);
                output_format(name, cnt);
                break;    
            case 'l':
                if (argv[optind] == NULL) {
                    cnt = file_name(fp, ".", name, flag);
                } else {
                    cnt = file_name(fp, argv[optind], name, flag);
                }
                str_sort(name, cnt);
                show(name, cnt);
                break;
            }
        }
     }
     return 0;
}

