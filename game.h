#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef enum { CLOSED, OPEN, FLAG } State;

typedef struct {
    char symbol;
    bool isMine;
    State state;
} Cell;

typedef struct {
    Cell** grid;
    int width;
    int height;
    int mines;
    bool firstMove;
    bool gameOver;
} Field;

typedef struct CellNode {
    int x;
    int y;
    Cell* cell;
    struct CellNode* next;
} CellNode;

CellNode* addCellNode(CellNode* list, Cell* cell, int x, int y);
Field* createField(int width, int height, int mines);
void printField(Field* field);
void freeField(Field* field);
Cell* getCell(Field* field, int x, int y);
int countMines(Field* field, int x, int y);
void populateField(Field* field, int x, int y);
void renderField(Field* field);
void openCellLoop(Field* field, CellNode* listToOpen);
bool openCell(Field* field, int x, int y);
void gameLoop();

#endif