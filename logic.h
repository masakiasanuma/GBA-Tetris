#ifndef LOGIC_H
#define LOGIC_H

#include "gba.h"
#include "myLib.h"

typedef struct {
    int gameOver;
    int points;
    int level;
    int levelTrack;
    int speed;
    int update;
    int placed;
    int tetris;
    u16 BOARD[BOARD_HEIGHT][BOARD_WIDTH];
    Block currBlock;
} AppState;

/*
* TA-TODO: Add any additional structs that you need for your app.
*
* For example, for a Snake game, one could be:
*
* typedef struct {
*   int heading;
*   int length;
*   int x;
*   int y;
* } Snake;
*
*/

// This function can initialize an unused AppState struct.
void initializeAppState(AppState *appState);

// This function will be used to process app frames.
AppState processAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow);

// If you have anything else you need accessible from outside the logic.c
// file, you can add them here. You likely won't.

#endif
