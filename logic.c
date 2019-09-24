#include "logic.h"

// Complete array of all block types and their transformation shapes.
int ALLBLOCKS[7][4][4][2] = {
    { // I
        {{1,0},{1,1},{1,2},{1,3}},
        {{0,1},{1,1},{2,1},{3,1}},
        {{2,0},{2,1},{2,2},{2,3}},
        {{0,2},{1,2},{2,2},{3,2}}
    },
    { // J
        {{1,0},{1,1},{1,2},{2,2}},
        {{0,1},{1,1},{2,1},{0,2}},
        {{1,0},{2,0},{2,1},{2,2}},
        {{2,0},{2,1},{1,1},{0,1}}
    },
    { // L
        {{1,0},{2,0},{1,1},{1,2}},
        {{0,1},{1,1},{2,1},{2,2}},
        {{2,0},{2,1},{2,2},{1,2}},
        {{0,0},{0,1},{1,1},{2,1}}
    },
    { // O
        {{1,1},{1,2},{2,1},{2,2}},
        {{1,1},{1,2},{2,1},{2,2}},
        {{1,1},{1,2},{2,1},{2,2}},
        {{1,1},{1,2},{2,1},{2,2}}
    },
    { // S
        {{2,0},{2,1},{1,1},{1,2}},
        {{0,0},{1,0},{1,1},{2,1}},
        {{2,0},{2,1},{1,1},{1,2}},
        {{0,1},{1,1},{1,2},{2,2}}
    },
    { // T
        {{1,0},{1,1},{1,2},{2,1}},
        {{0,1},{1,1},{2,1},{1,2}},
        {{2,0},{2,1},{2,2},{1,1}},
        {{1,0},{0,1},{1,1},{2,1}}
    },
    { // Z
        {{1,0},{1,1},{2,1},{2,2}},
        {{0,1},{1,1},{1,0},{2,0}},
        {{1,0},{1,1},{2,1},{2,2}},
        {{0,2},{1,1},{1,2},{2,1}}
    },
};

static Block makeBlock(int r, int c, int type, int rotation) {
    Block block;

    DMA[3].src = 0;
    DMA[3].dst = &(block.grid[0][0]);
    DMA[3].cnt = 16 | DMA_SOURCE_FIXED | DMA_32 | DMA_ON;

    block.r = r;
    block.c = c;
    if (rotation == 4) {
        rotation = 0;
    }
    if (rotation == -1) {
        rotation = 3;
    }
    block.rotation = rotation;
    block.type = type;
    for (int i = 0; i < 4; i++) {
        block.grid[ALLBLOCKS[type][rotation][i][0]][ALLBLOCKS[type][rotation][i][1]] = 1;
    }

    if (type == 0) {
        block.color = RED;
    } else if (type == 1) {
        block.color = GREEN;
    } else if (type == 2) {
        block.color = BLUE;
    } else if (type == 3) {
        block.color = MAGENTA;
    } else if (type == 4) {
        block.color = CYAN;
    } else if (type == 5) {
        block.color = YELLOW;
    } else if (type == 6) {
        block.color = GRAY;
    }
    return block;
}

static int legalMove(int r, int c, Block curr, u16 board[BOARD_HEIGHT][BOARD_WIDTH]) {
    curr.r += r;
    curr.c += c;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (curr.grid[i][j] == 1) {
                if (i + curr.r >= BOARD_HEIGHT
                    || j + curr.c >= BOARD_WIDTH
                    || j + curr.c < 0
                    || i + curr.r < 0) {
                    return 0;
                }
                if (board[curr.r + i][curr.c + j] != 0) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

static int legalRotate(Block curr, u16 board[BOARD_HEIGHT][BOARD_WIDTH], int clock) {
    Block testBlk = makeBlock(curr.r, curr.c, curr.type, curr.rotation + clock);
    return legalMove(0, 0, testBlk, board);
}

static int instantDown(Block curr, u16 board[BOARD_HEIGHT][BOARD_WIDTH]) {
    int down = 0;
    while (legalMove(1, 0, curr, board)) {
        curr.r += 1;
        down++;
    }
    return down;
}

void initializeAppState(AppState* appState) {
    // TA-TODO: Initialize everything that's part of this AppState struct here.
    // Suppose the struct contains random values, make sure everything gets
    // the value it should have when the app begins.
    DMA[3].src = 0;
    DMA[3].dst = &(appState->BOARD[0][0]);
    DMA[3].cnt = (BOARD_WIDTH * BOARD_HEIGHT) | DMA_SOURCE_FIXED | DMA_32 | DMA_ON;
    appState->gameOver = 0;
    appState->points = 0;
    appState->level = 1;
    appState->levelTrack = 0;
    appState->speed = 60;
    appState->update = 0;
    appState->placed = 0;
    appState->tetris = 0;
    appState->currBlock = makeBlock(-1, 3, randint(0, 7), 0);

}

// TA-TODO: Add any process functions for sub-elements of your app here.
// For example, for a snake game, you could have a processSnake function
// or a createRandomFood function or a processFoods function.
//
// e.g.:
// static Snake processSnake(Snake* currentSnake);
// static void generateRandomFoods(AppState* currentAppState, AppState* nextAppState);

// This function processes your current app state and returns the new (i.e. next)
// state of your application.
AppState processAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow) {
    /* TA-TODO: Do all of your app processing here. This function gets called
     * every frame.
     *
     * To check for key presses, use the KEY_JUST_PRESSED macro for cases where
     * you want to detect each key press once, or the KEY_DOWN macro for checking
     * if a button is still down.
     *
     * To count time, suppose that the GameBoy runs at a fixed FPS (60fps) and
     * that VBlank is processed once per frame. Use the vBlankCounter variable
     * and the modulus % operator to do things once every (n) frames. Note that
     * you want to process button every frame regardless (otherwise you will
     * miss inputs.)
     *
     * Do not do any drawing here.
     *
     * TA-TODO: VERY IMPORTANT! READ THIS PART.
     * You need to perform all calculations on the currentAppState passed to you,
     * and perform all state updates on the nextAppState state which we define below
     * and return at the end of the function. YOU SHOULD NOT MODIFY THE CURRENTSTATE.
     * Modifying the currentAppState will mean the undraw function will not be able
     * to undraw it later.
     */

    AppState nextAppState = *currentAppState;

    nextAppState.update += 1;

    if (nextAppState.update == nextAppState.speed) {
        if (!legalMove(1, 0, nextAppState.currBlock, nextAppState.BOARD)) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    if (nextAppState.currBlock.grid[i][j]) {
                        nextAppState.BOARD[nextAppState.currBlock.r + i][nextAppState.currBlock.c + j]
                            = nextAppState.currBlock.color;
                    }
                }
            }

            int begin = nextAppState.currBlock.r;
            int end = begin + 4;
            int fill = 0;
            int full;

            nextAppState.currBlock = makeBlock(-1, 3, randint(0, 7), 0);

            if (!legalMove(0, 0, nextAppState.currBlock, nextAppState.BOARD)) {
                nextAppState.gameOver = 1;
                return nextAppState;
            }

            for (int i = begin; i < end; i++) {
                full = 1;
                for (int j = 0; j < BOARD_WIDTH; j++) {
                    if (nextAppState.BOARD[i][j] == 0) {
                        full = 0;
                        break;
                    }
                }
                if (full) {
                    for (int j = 0; j < BOARD_WIDTH; j++) {
                        for (int k = i; k >= 0; k--) {
                            nextAppState.BOARD[k][j] = (k == 0) ? 0 : nextAppState.BOARD[k - 1][j];
                        }
                    }
                    fill++;
                }
            }

            nextAppState.points += fill;
            nextAppState.levelTrack += fill;

            if (fill == 4) {
                nextAppState.tetris = 1;
            } else {
                nextAppState.tetris = 0;
            }

            if (nextAppState.levelTrack >= 10) {
                nextAppState.level += 1;
                nextAppState.levelTrack = 0;
                if (nextAppState.level == 46) {
                    nextAppState.gameOver = 1;
                    return nextAppState;
                }
            }

            nextAppState.speed = randint(5, 50 - nextAppState.level);
            nextAppState.update = 0;
            nextAppState.placed = 1;
            return nextAppState;
        }
        nextAppState.currBlock.r += 1;
        nextAppState.update = 0;
    }

    if (KEY_DOWN(BUTTON_LEFT, keysPressedNow) && !KEY_DOWN(BUTTON_LEFT, keysPressedBefore)
        && legalMove(0, -1, nextAppState.currBlock, nextAppState.BOARD)) {
        nextAppState.currBlock.c -= 1;
    } else if (KEY_DOWN(BUTTON_RIGHT, keysPressedNow) && !KEY_DOWN(BUTTON_RIGHT, keysPressedBefore)
        && legalMove(0, 1, nextAppState.currBlock, nextAppState.BOARD)) {
        nextAppState.currBlock.c += 1;
    } else if (KEY_DOWN(BUTTON_DOWN, keysPressedNow) && !KEY_DOWN(BUTTON_DOWN, keysPressedBefore)
        && legalMove(1, 0, nextAppState.currBlock, nextAppState.BOARD)) {
        nextAppState.currBlock.r += 1;
    } else if (KEY_DOWN(BUTTON_UP, keysPressedNow) && !KEY_DOWN(BUTTON_UP, keysPressedBefore)) {
        nextAppState.currBlock.r += instantDown(nextAppState.currBlock, nextAppState.BOARD);
    } else if (KEY_DOWN(BUTTON_A, keysPressedNow) && !KEY_DOWN(BUTTON_A, keysPressedBefore)
        && legalRotate(nextAppState.currBlock, nextAppState.BOARD, 1)) {
        nextAppState.currBlock = makeBlock(nextAppState.currBlock.r,
                                           nextAppState.currBlock.c,
                                           nextAppState.currBlock.type,
                                           nextAppState.currBlock.rotation + 1);
    } else if (KEY_DOWN(BUTTON_B, keysPressedNow) && !KEY_DOWN(BUTTON_B, keysPressedBefore)
        && legalRotate(nextAppState.currBlock, nextAppState.BOARD, -1)) {
        nextAppState.currBlock = makeBlock(nextAppState.currBlock.r,
                                           nextAppState.currBlock.c,
                                           nextAppState.currBlock.type,
                                           nextAppState.currBlock.rotation - 1);
    }

    return nextAppState;
}
