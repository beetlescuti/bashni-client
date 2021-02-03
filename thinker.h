#include "sharedMemory.h"

#ifndef thinker
#define thinker

extern int shmid_for_info;

void think(int * shmid_ptr);
char toppiece(char board[8][8][25], int x, int y);
char** possiblemoves(char board[8][8][25]);
char* possibletowermoves(char board[8][8][25], int x, int y, int direction);
void calc_move(char board[8][8][25], int x, int y, int pos_x, int pos_y);
char* translate_pos(int x, int y);

#endif
