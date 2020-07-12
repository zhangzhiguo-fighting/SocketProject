#include "head.h"
#include "game.h"
#define MAX 50

extern struct User *rteam, *bteam;
extern WINDOW *Football;
extern WINDOW *Score;
extern struct Bpoint ball;
extern struct BallStatus ball_status;
extern struct Score score;

void re_draw_player(int team, char *name, struct Point *loc) {
    char p = 'k';
    if (team)
        wattron(Football, COLOR_PAIR(6));
    else
        wattron(Football, COLOR_PAIR(2));
    w_gotoxy_putc(Football, loc->x, loc->y, p);
    w_gotoxy_puts(Football, loc->x + 1, loc->y - 1, name);
}

void re_draw_team(struct User *team) {
    for (int i = 0; i < MAX; i++) {
        if (!team[i].online) continue;
        re_draw_player(team[i].team, team[i].name, &team[i].loc);
    }
}

void re_draw_ball() {
    double t = 0.1;
    if (ball_status.carry == 0) {    
        if((int)ball_status.v.x != 0 ||(int) ball_status.v.y != 0) {
            ball.x += ball_status.v.x * t + ball_status.a.x * 0.5 * 0.01;
            ball.y += ball_status.v.y * t + ball_status.a.y * 0.5 * 0.01;
            ball_status.v.x += ball_status.a.x * t;
            ball_status.v.y += ball_status.a.y * t;
        }
        if (abs(ball_status.v.x) < 0.5) {
		    ball_status.v.x = ball_status.a.x = 0;
	    }
	    if (abs(ball_status.v.y) < 0.5) {
		    ball_status.v.y = ball_status.a.y = 0;
	    }
        if (ball_status.v.x || ball_status.v.y) {
		    char tmp[512] = {0};
		    sprintf(tmp, "a(%lf, %lf) v(%lf, %lf)", ball_status.a.x, ball_status.a.y, ball_status.v.x, ball_status.v.y);
		    Show_Message( , NULL, tmp, 1);
	    }
    } else if (ball_status.carry == 1){ 
        int px, py;
        for (int i = 0; i < MAX; i++) {
            if (rteam[i].carry == 1) {
                px = rteam[i].loc.x;
                py = rteam[i].loc.y;
            } 
            if (bteam[i].carry == 1) {
                px = bteam[i].loc.x;
                py = bteam[i].loc.y;
            }
        }

        int dir[8][2] = {1, 0, -1, 0,
                     1, 1, 1, -1,  
                     0, 1, 0, -1
                     -1, 1, -1, -1
        };
        ball.y =  py + dir[rand() % 8][rand() % 2];
        ball.x =  px + dir[rand() % 8][rand() % 2];
        //char temp[512];
        //sprintf(temp, "%s", "coming");
        //Show_Message(, NULL, temp, 1);
    }

	/*if (ball.x <= 0 || ball.x >= 114 || ball.y <= 0 || ball.y >= 24) {
		ball.x = court.width / 2;
		ball.y = court.heigth / 2;
		ball_status.v.x = ball_status.v.y = 0;
		ball_status.a.x = ball_status.a.y = 0;
	}*/

    //Boundary judgment
    if (ball.x < 0) {
		if (ball.y >= (2 * court.heigth / 5) && ball.y <= (3 * court.heigth / 5)) {
			ball.x = court.width / 2;
			ball.y = court.heigth / 2;
			score.red += 1;
			char tmp[512] = {0};
			sprintf(tmp, "%s get 1 score", ball_status.name);
			Show_Message( , NULL, tmp, 1);

		} else {
			ball.x = 1;
		}
		ball_status.v.x = ball_status.v.y = 0;
		ball_status.a.x = ball_status.a.y = 0;
	}
	if (ball.x > court.width - 1) {
		if (ball.y >= (2 * court.heigth / 5) && ball.y <= (3 * court.heigth / 5)) {
			ball.x = court.width / 2;
			ball.y = court.heigth / 2;
			score.blue += 1;
			char tmp[512] = {0};
			sprintf(tmp, "%s get 1 score", ball_status.name);
			Show_Message( , NULL, tmp, 1);

		} else {
			ball.x = court.width - 2;
		}
		ball_status.v.x = ball_status.v.y = 0;
		ball_status.a.x = ball_status.a.y = 0;
	}
	if (ball.y <= 0) {
		ball.y = 1;
		ball_status.v.x = ball_status.v.y = 0;
		ball_status.a.x = ball_status.a.y = 0;
	}
	if (ball.y >= court.heigth - 1) {
		ball.y = court.heigth - 2;
		ball_status.v.x = ball_status.v.y = 0;
		ball_status.a.x = ball_status.a.y = 0;
	}
    
    w_gotoxy_putc(Football, (int)ball.x, (int)ball.y, 'O');
}

void re_draw_goal() {
    for (int i = (2 * court.heigth / 5); i <= (3 * court.heigth / 5); i++) {
		w_gotoxy_puts(Football_t, 1, i + 1, "x");
		w_gotoxy_puts(Football_t, court.width + 2, i + 1, "x");
	}
}

void re_draw_score() {
    mvwprintw(Score, 1, 1, "%s", "RED TEAM:");
    mvwprintw(Score, 1, 12, "%d", score.red);
    mvwprintw(Score, 3, 1, "%s", "BLUE TEAM:");
    mvwprintw(Score, 3, 12, "%d", score.blue);
    wrefresh(Score);
}

void re_draw() {
    werase(Football_t);
    box(Football_t, 0, 0);
    box(Football, 0, 0);
    re_draw_team(rteam);
    re_draw_team(bteam);
    re_draw_ball();
	//re_draw_goal();
    re_draw_score();
    wrefresh(Football_t);
}
