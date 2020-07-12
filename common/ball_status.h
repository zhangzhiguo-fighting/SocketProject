#ifndef _BALL_STATUS_H
#define _BALL_STATUS_H
int can_kick(struct Point *loc, int strength);
int can_stop(struct Point *loc);
int can_carry(struct Point *loc);
#endif
