#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "game.h"

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
    field->gameOver = false;

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
    for (int i = 0; i < field->height; i++) {
        for (int j = 0; j < field->width; j++) {
            Cell cell = field->grid[i][j];
            printf("%c ", cell.state == OPEN ? cell.symbol : '#');
        }
        printf("\n");
    }
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
    if (cell == NULL || cell->state != CLOSED) {
        return false;
    }

    if (field->firstMove) {
        populateField(field, x, y);
        renderField(field);
        field->firstMove = false;
    }

    if (cell->isMine) {
        field->gameOver = true;
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


void gameLoop() {
    Field* field = createField(50, 50, 250);

    openCell(field, 25, 25);
    printField(field);

    freeField(field);
}
