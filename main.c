#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "minesweeper.h"

int main() {
    srand(time(NULL));
    gameLoop();
    return 0;
}
