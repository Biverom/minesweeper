#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef enum { CLOSED, OPEN, FLAG } CellState;
typedef enum { INGAME, WIN, LOOSE, EXIT } FieldState;

typedef struct {
    char symbol;
    bool isMine;
    CellState state;
} Cell;

typedef struct {
    Cell** grid;
    int width;
    int height;
    int mines;
    bool firstMove;
    FieldState state;
    unsigned int seed;
    FILE* readRecord;
    FILE* writeRecord;
    long score;
} Field;

typedef struct CellNode {
    int x;
    int y;
    Cell* cell;
    struct CellNode* next;
} CellNode;

CellNode* addCellNode(CellNode* list, Cell* cell, int x, int y);
Field* createField(int width, int height, int mines, unsigned int seed);
Field* createFieldFromRecord(FILE* record);

void printField(Field* field);
void freeField(Field* field);
Cell* getCell(Field* field, int x, int y);
int countMines(Field* field, int x, int y);
void populateField(Field* field, int x, int y);
void renderField(Field* field);
int countClosedCells(Field* field);
void checkWin(Field* field);
void openCellLoop(Field* field, CellNode* listToOpen);
bool openCell(Field* field, int x, int y);
bool toggleFlagOnCell(Field* field, int x, int y);

bool openCellCommand(Field* field, const char* action, bool isRecord);
bool toggleFlagCommand(Field* field, const char* action, bool isRecord);
bool leaveGameCommand(Field* field, const char* action, bool isRecord);
void runUserCommand(Field* field, char* action, bool isRecord);

void minesweeperLoop(Field* field);

#endif