#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "minesweeper.h"

void playGame() {
    Field *field = NULL;
    bool back = false;

    while (true) {
        printf("\nChoose difficulty:\n[E] Easy\n[M] Medium\n[H] Hard\n[C] Custom\n[B] Back\n");
        char input[100];
        fgets(input, sizeof(input), stdin);

        char command;
        bool success = false;
        if (sscanf(input, "%c", &command) == 1) {
            switch (command) {
                case 'e':
                case 'E':
                    field = createField(9, 9, 10);
                    success = true;
                    break;
                case 'm':
                case 'M':
                    field = createField(16, 16, 40);
                    success = true;
                    break;
                case 'h':
                case 'H':
                    field = createField(30, 16, 99);
                    success = true;
                    break;
                case 'c':
                case 'C':
                    {
                        int rows, cols, numMines;
                        printf("Enter the number of rows, columns, and mines: ");
                        if (scanf("%d %d %d", &rows, &cols, &numMines) != 3 || rows <= 0 || cols <= 0 || numMines <= 0 || numMines >= rows * cols) {
                            printf("Invalid input!\n");
                            while(getchar() != '\n');
                            break;
                        }
                        while(getchar() != '\n');
                        field = createField(rows, cols, numMines);
                        success = true;
                    }
                    break;
                case 'b':
                case 'B':
                    success = true;
                    break;
            }
        }

        if (success) {
            break;
        } else {
            printf("Invalid action!\n");
        }
    }

    if (field != NULL) {
        minesweeperLoop(field);
    }
}

void gameLoop() {
    bool close = false;
    while (!close)
    {
        printf("\nWelcome to Minesweeper!\nWhat would you like to do?\n[P] Play\n[R] Watch replay\n[X] Exit\n");
        char input[100];
        fgets(input, sizeof(input), stdin);

        char command;
        if (sscanf(input, "%c", &command) == 1) {
            switch (command) {
                case 'p':
                case 'P':
                    playGame();
                    break;
                case 'r':
                case 'R':
                    printf("Replay\n");
                    break;
                case 'x':
                case 'X':
                    printf("Goodbye!\n");
                    close = true;
            }
        }
    }
}