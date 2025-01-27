#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#define LEADERBOARD_FILE "saves/leaderboard/global.mslb"

typedef struct {
    char name[16];
    long score;
} LeaderboardEntry;

void leaderboardPrint(int top_n);
void leaderboardAddScore(const char *name, long score);

#endif
