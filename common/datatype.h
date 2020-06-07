#ifdef _DATATYPE_H
#define _DATATYPE_H

struct LogRequest{
    char name[20]; //Nmae of user
    int team; //0 READ 1 BLUE
    char msg[512];
};

struct LogResponse{
    int type; //0 success 1 failed
    char msg[512];
};

struct Point{
    int x;
    int y;
};

struct User{
    int team;
    char name[20];
    int flag; //未效应次数,判断是否在线
    struct sockaddr_in addr;
    struct Point loc;
};

struct Map {
    int width; //Column
    int heigth; //Row
    struct Point start; //已知起点，长，宽，便可以绘制一个矩形
    int gate_width;
    int gate_heigth;
};

//Operate the ball
struct Ctrl {
    int carry;
    int kick;
};


struct TransMsg{
    int dirx;
    int diry;
    struct Ctrl ctrl;
};

#endif
