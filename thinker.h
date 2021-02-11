#include "sharedMemory.h"

#ifndef thinker
#define thinker

extern int shmid_for_info;

void think(int * shmid_ptr);
int bestmove();
char toppiece(char board[8][8][25], int x, int y);
char** possiblemoves(char board[8][8][25]);
void possibletowermoves(char board[8][8][25], int x, int y, int direction);
void possibletowermoves_queen(char board[8][8][25], int x, int y, int direction);
char* translate_pos(int x, int y);
int* moveindirection(int direction, int x, int y);

#endif
