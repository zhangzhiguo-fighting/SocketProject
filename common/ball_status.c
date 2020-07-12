#include "head.h"

extern WINDOW *Message;
extern struct Bpoint ball; //Ball position
extern struct BallStatus ball_status;

int can_kick(struct Point *loc, int strength) {
    //int px = loc->x - 2, py = loc->y - 1; //将人和球的坐标对齐
    int px = loc->x, py = loc->y; //将人和球的坐标对齐
    if (abs(px - (int)ball.x) <= 2 && abs(py - (int)ball.y <= 2)) {
        if (px == (int)ball.x && py == (int)ball.y) return 0; //Can't play football while standing on the ball
        double dx = (ball.x - px) / pow(pow((ball.x - px), 2) + pow((ball.y - py), 2), 0.5);
        double dy = (ball.y - py) / pow(pow((ball.x - px), 2) + pow((ball.y - py), 2), 0.5);
        ball_status.a.x = -3 * dx; //a of air resistans
        ball_status.a.y = -3 * dy;
        ball_status.v.x = strength * 8 * dx; // t = 0.2 v0 = 40
        ball_status.v.y = strength * 8 * dy;
        return 1;
    }
    return 0;
}

int can_stop(struct Point *loc) {
    int px = loc->x;
    int py = loc->y;
    if (abs(px - (int)ball.x) <= 2 && abs(py - (int)ball.y) <= 2) {
        ball_status.a.x = ball_status.a.y = 0;
        ball_status.v.x = ball_status.v.y = 0;
        return 1;
    }
    return 0;
}

int can_carry(struct Point *loc) {
    int px = loc->x;
    int py = loc->y;
    if (abs(px - (int)ball.x) <= 2 && abs(py - (int)ball.y) <= 2) {
        ball_status.v.x = ball_status.v.y = 0;
        ball_status.a.x = ball_status.a.y = 0;
        return 1;
    }
    return 0;
}
