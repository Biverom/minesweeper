#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "utils.h"
#include "game.h"

#include "minesweeper.h"

CellNode* addCellNode(CellNode* list, Cell* cell, int x, int y) {
    CellNode* cellNode = list;
    while (cellNode != NULL) {
        if (cellNode->cell == cell) {
            return list;
        }
        cellNode = cellNode->next;
    }
    CellNode* newCellNode = (CellNode*)malloc(sizeof(CellNode));
    newCellNode->x = x;
    newCellNode->y = y;
    newCellNode->cell = cell;
    newCellNode->next = list;
    return newCellNode;
}

Field* createField(int width, int height, int mines, unsigned int seed) {
    Field* field = (Field*)malloc(sizeof(Field));
    field->width = width;
    field->height = height;
    field->mines = mines;
    field->firstMove = true;
    field->state = INGAME;
    field->score = 0;

    field->seed = seed;

    field->readRecord = NULL;

    char path[100] = "saves/records/latest.msrec";
    fixpath(path);
    field->writeRecord = fopen(path, "w");

    field->grid = malloc(height * sizeof(Cell*));
    for (int i = 0; i < height; i++) {
        field->grid[i] = malloc(width * sizeof(Cell));
        for (int j = 0; j < width; j++) {
            field->grid[i][j].state = CLOSED;
            field->grid[i][j].isMine = false;
            field->grid[i][j].symbol = '.';
        }
    }

    fprintf(field->writeRecord, "%d %d %d\n", width, height, mines);
    fprintf(field->writeRecord, "%d\n", field->seed);

    return field;
}

Field* createFieldFromRecord(FILE* record) {
    int width, height, mines;
    unsigned int seed;

    fscanf(record, "%d %d %d\n", &width, &height, &mines);
    fscanf(record, "%d\n", &seed);

    Field* field = createField(width, height, mines, seed);

    field->seed = seed;
    field->readRecord = record;

    return field;
}

void printField(Field* field) {

    int widthDigits = numPlaces(field->width - 1);
    int heightDigits = numPlaces(field->height - 1);

    for (int i = widthDigits; i > 0; i--) {
        printf("%*s", heightDigits + 3, "");
        for (int j = 0; j < field->width; j++) {
            printf("%d ", getNthDigit(j, i));
        }
        printf("\n");
    }
    printf("\n");

    for (int i = 0; i < field->height; i++) {
        printf("%0*d   ", heightDigits, i);
        for (int j = 0; j < field->width; j++) {
            Cell cell = field->grid[i][j];
            if (cell.state == CLOSED) {
                printf("# ");
            } else if (cell.state == FLAG) {
                printf("F ");
            } else {
                printf("%c ", cell.symbol);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void freeField(Field* field) {
    for (int i = 0; i < field->height; i++) {
        free(field->grid[i]);
    }
    free(field->grid);
    if (field->readRecord != NULL) {
        fclose(field->readRecord);
    }
    if (field->writeRecord != NULL) {
        fclose(field->writeRecord);
    }
    free(field);
}

Cell* getCell(Field* field, int x, int y) {
    if (x < 0 || x >= field->width || y < 0 || y >= field->height) {
        return NULL;
    }
    return &field->grid[y][x];
}

int countMines(Field* field, int x, int y) {
    int mines = 0;
    for (int i = y - 1; i <= y + 1; i++) {
        for (int j = x - 1; j <= x + 1; j++) {
            Cell* cell = getCell(field, j, i);
            if (cell != NULL && cell->isMine) {
                mines++;
            }
        }
    }
    return mines;
}

void populateField(Field* field, int x, int y) {
    srand(field->seed);
    int mines = 0;
    while (mines < field->mines) {
        int i = rand() % field->height;
        int j = rand() % field->width;

        if (field->grid[i][j].isMine || (i >= y - 1 && i <= y + 1 && j >= x - 1 && j <= x + 1)) {
            continue;
        }

        field->grid[i][j].isMine = true;
        mines++;
    }
}

void renderField(Field* field) {
    for (int i = 0; i < field->height; i++) {
        for (int j = 0; j < field->width; j++) {
            if (field->grid[i][j].isMine) {
                field->grid[i][j].symbol = '*';
            } else {
                int mines = countMines(field, j, i);
                if (mines == 0) {
                    field->grid[i][j].symbol = '.';
                } else {
                    field->grid[i][j].symbol = mines + '0';
                }
            }
        }
    }
}

int countCellsWithState(Field* field, CellState state) {
    int closedCells = 0;
    for (int i = 0; i < field->height; i++) {
        for (int j = 0; j < field->width; j++) {
            if (field->grid[i][j].state == state) {
                closedCells++;
            }
        }
    }
    return closedCells;
}

void checkWin(Field* field) {
    int closedCells = (field->width * field->height) - countCellsWithState(field, OPEN);
    if (closedCells <= field->mines) {
        field->state = WIN;
    }
}

void givePoints(Field* field) {
    int scoreMult = (100*field->mines)/(field->width*field->height);
    field->score += scoreMult;
}

void openCellLoop(Field* field, CellNode* listToOpen) {
    CellNode* current = listToOpen;
    CellNode* nextList = NULL;

    while (current != NULL) {
        Cell* cell = current->cell;
        int x = current->x;
        int y = current->y;

        field->grid[y][x].state = OPEN;
        givePoints(field);

        if (countMines(field, x, y) == 0) {
            for (int i = y - 1; i <= y + 1; i++) {
                for (int j = x - 1; j <= x + 1; j++) {
                    Cell* neighbor = getCell(field, j, i);
                    if (neighbor != NULL && neighbor->state == CLOSED) {
                        neighbor->state = OPEN;
                        givePoints(field);
                        nextList = addCellNode(nextList, neighbor, j, i);
                    }
                }
            }
        }

        CellNode* next = current->next;
        free(current);
        current = next;
    }

    if (nextList != NULL) {
        openCellLoop(field, nextList);
    }
}

bool openCell(Field* field, int x, int y) {
    Cell* cell = getCell(field, x, y);

    if (cell == NULL) {
        return false;
    }

    if (cell->state == OPEN) {
        bool res = false;
        for (int i = y - 1; i <= y + 1; i++) {
            for (int j = x - 1; j <= x + 1; j++) {
                if (i == y && j == x) {
                    continue;
                }
                Cell* neighbor = getCell(field, j, i);
                if (neighbor != NULL && neighbor->state == CLOSED) {
                    res |= openCell(field, j, i);
                }
            }
        }
        return res;
    }


    if (field->firstMove) {
        populateField(field, x, y);
        renderField(field);
        field->firstMove = false;
    }

    if (cell->isMine) {
        cell->state = OPEN;
        field->state = LOOSE;
        return true;
    }

    CellNode* listToOpen = (CellNode*)malloc(sizeof(CellNode));
    listToOpen->x = x;
    listToOpen->y = y;
    listToOpen->cell = cell;
    listToOpen->next = NULL;

    openCellLoop(field, listToOpen);

    return true;
}

bool toggleFlagOnCell(Field* field, int x, int y) {
    Cell* cell = getCell(field, x, y);
    if (cell == NULL || cell->state == OPEN) {
        return false;
    }

    cell->state = cell->state == FLAG ? CLOSED : FLAG;
    return true;
}

bool openCellCommand(Field* field, const char* action, bool isRecord) {
    int x, y;
    if (sscanf(action, "%*c %d %d", &x, &y) == 2) {
        bool success = openCell(field, x, y);
        if (success) checkWin(field);
        return success;
    }
    return false;
}

bool toggleFlagCommand(Field* field, const char* action, bool isRecord) {
    int x, y;
    if (sscanf(action, "%*c %d %d", &x, &y) == 2) {
        return toggleFlagOnCell(field, x, y);
    }
    return false;
}

bool leaveGameCommand(Field* field, const char* action, bool isRecord) {
    if (isRecord) {
        printf("Player exited the game\n");
        return false;
    }
    field->state = EXIT;
    return true;
}

void runUserCommand(Field* field, char* action, bool isRecord) {
    char command;
    bool success = false;

    if (sscanf(action, "%c", &command) == 1) {
        switch (command) {
            case 'o':
            case 'O':
                success = openCellCommand(field, action, isRecord);
                break;

            case 'f':
            case 'F':
                success = toggleFlagCommand(field, action, isRecord);
                break;

            case 'x':
            case 'X':
                success = leaveGameCommand(field, action, isRecord);
                break;
        }
    }

    if (!success && !isRecord) {
        printf("Invalid command!\n");
    }
    else {
        fprintf(field->writeRecord, "%s", action);
    }
}


void minesweeperLoop(Field* field) {
    while (field->state == INGAME) {
        printField(field);

        char action[16] = "";
        bool isRecord = false;
        if ((field->readRecord != NULL) && !feof(field->readRecord)) {
            if (fgets(action, sizeof(action), field->readRecord) != NULL) {
                printf("Enter command (recorded): %s\n", action);
                isRecord = true;
            }
        } 
        if (!isRecord) {
            printf("Enter command: ");
            fgets(action, sizeof(action), stdin);
        }
        runUserCommand(field, action, isRecord);
        printf("\n");
    }
    printField(field);
    printf("Game over! You %s the game!\n", (field->state == EXIT) ? "left" : ((field->state == WIN) ? "won" : "lost"));
    printf("Your score: %ld\n", field->score);
}
