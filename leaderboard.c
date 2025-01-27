#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leaderboard.h"

void leaderboardPrint(int top_n) {
    FILE *file = fopen(LEADERBOARD_FILE, "r");
    if (!file) {
        printf("Error: Could not open leaderboard file.\n");
        return;
    }

    LeaderboardEntry *entries = NULL;
    int count = 0;

    LeaderboardEntry entry;
    while (fscanf(file, "%16s %ld", entry.name, &entry.score) == 2) {
        entries = realloc(entries, (count + 1) * sizeof(LeaderboardEntry));
        entries[count++] = entry;
    }
    fclose(file);

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (entries[i].score < entries[j].score) {
                LeaderboardEntry temp = entries[i];
                entries[i] = entries[j];
                entries[j] = temp;
            }
        }
    }

    printf("Leaderboard:\n");
    int limit = (top_n == -1 || top_n > count) ? count : top_n;
    for (int i = 0; i < limit; i++) {
        printf("%s: %ld\n", entries[i].name, entries[i].score);
    }

    free(entries);
}

void leaderboardAddScore(const char *name, long score) {
    FILE *file = fopen(LEADERBOARD_FILE, "a");
    if (!file) {
        printf("Error: Could not open leaderboard file.\n");
        return;
    }

    fprintf(file, "%s %ld\n", name, score);
    fclose(file);
}