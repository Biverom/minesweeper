#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "utils.h"

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

Field* createField(int width, int height, int mines) {
    Field* field = (Field*)malloc(sizeof(Field));
    field->width = width;
    field->height = height;
    field->mines = mines;
    field->firstMove = true;
    field->state = INGAME;

    field->grid = malloc(height * sizeof(Cell*));
    for (int i = 0; i < height; i++) {
        field->grid[i] = malloc(width * sizeof(Cell));
        for (int j = 0; j < width; j++) {
            field->grid[i][j].state = CLOSED;
            field->grid[i][j].isMine = false;
            field->grid[i][j].symbol = '.';
        }
    }

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

void openCellLoop(Field* field, CellNode* listToOpen) {
    CellNode* current = listToOpen;
    CellNode* nextList = NULL;

    while (current != NULL) {
        Cell* cell = current->cell;
        int x = current->x;
        int y = current->y;

        field->grid[y][x].state = OPEN;

        if (countMines(field, x, y) == 0) {
            for (int i = y - 1; i <= y + 1; i++) {
                for (int j = x - 1; j <= x + 1; j++) {
                    Cell* neighbor = getCell(field, j, i);
                    if (neighbor != NULL && neighbor->state == CLOSED) {
                        neighbor->state = OPEN;
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
        field->state = LOOSE;
        return false;
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

void runUserCommand(Field* field, char* action) {
    int x, y;
    char command;
    bool success = false;
    if (sscanf(action, "%c %d %d", &command, &x, &y) == 3) {
        if (command == 'o') {
            success = openCell(field, x, y);
            checkWin(field);
        } else if (command == 'f') {
            success = toggleFlagOnCell(field, x, y);
        }
    } else if (sscanf(action, "%c", &command) == 1) {
        if (command == 'e') {
            field->state = EXIT;
            success = true;
        }
    }
    if (!success) {
        printf("Invalid command!\n");
    }
}

void gameLoop() {
    Field* field = createField(4, 4, 1);

    while (field->state == INGAME) {
        printField(field);

        char action[16] = "";
        printf("Enter command: ");
        fgets(action, sizeof(action), stdin);
        runUserCommand(field, action);
        printf("\n");
    }
    printField(field);
    printf("Game over! You %s!\n", field->state == WIN ? "won" : "lost");

    freeField(field);
}
