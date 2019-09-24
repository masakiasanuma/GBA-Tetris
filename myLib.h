#ifndef MYLIB_H
#define MYLIB_H

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define INIT_ROW 10
#define INIT_COL 50
#define PIX 7

#define BLOCK_I 0
#define BLOCK_J 1
#define BLOCK_L 2
#define BLOCK_O 3
#define BLOCK_S 4
#define BLOCK_T 5
#define BLOCK_Z 6

typedef struct {
    int r;
    int c;
    int rotation;
    int type;
    int grid[4][4];
    u16 color;
} Block;

#endif
